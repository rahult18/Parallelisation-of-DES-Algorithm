#include <bits/stdc++.h> 
#include <omp.h>
#include <fstream>
#include "header_files/matrices.h"
#include "header_files/conversions.h"
using namespace std;

//this function is used for permutation i.e. transposition 
//the bit positions are shuffled
string permutation(string str, int* arr, int n) 
{ 
	string res = ""; 
	for (int i = 0; i < n; i++) { 
		res += str[arr[i] - 1]; 
	} 
	return res; 
} 

//this function is used to perform 1 bit left shift and also 2 bit left shift
string shift_left(string k, int shifts) 
{ 
	string s = ""; 
	for (int i = 0; i < shifts; i++) { 
		//j=28 bcoz the 56 bit key is divided into LPT and RPT of 28 bits each
		for (int j = 1; j < 28; j++) { 
			s += k[j]; 
		} 
		s += k[0]; 
		k = s; 
		s = ""; 
	} 
	return k; 
} 

//this function is used to perform a simple XOR operation on 2 strings
string perform_xor(string str1, string str2) 
{ 
	string xored = ""; 
	for (int i = 0; i < str1.size(); i++) { 
		if (str1[i] == str2[i])
			xored += "0"; 
		else
			xored += "1";
	} 
	return xored; 
} 

//this function is used to encrypt the plain text to cipher text using the keys
string encryption(string pt,vector<string> keys) 
{ 
	// converts the string to binary 
	pt = string_to_binary(pt); 

	// Step 1: Initial Permutation Process
	pt = permutation(pt, initialPermutation, 64); 

	// Splitting of 64bits plain text to LPT and RPT of 32 bits each
	string left = pt.substr(0, 32); 
	string right = pt.substr(32, 32);  

	//performing 16 rounds 
	for (int i = 0; i < 16; i++) { 
		// Step 2.2: Exapansion Permutation 
		string expandedRPT = permutation(right, dBox, 48);

		// Step 2.2: XOR of 48-bit RoundKey and 48-bit expandedRPT
		string x = perform_xor(keys[i],expandedRPT); 

		// Step 2.3: S-boxes 
		//a string array (result) for storing the 4 bits outputs for 8 blocks of 6 bit input (total 48 bits)
		string result[8]; 
		//string res stores the final result from s box. i.e. concat all the result array elements
		string res = "";
		int row[8],col[8],val[8];

		//nested parallelism
		#pragma omp parallel for num_threads(2) schedule(static)
		//i<8 as 8 blocks are present
			for (int i = 0; i < 8; i++) {
				//the value of '0' is 48, '1' is 49 and so on. but since we are referring the matrix index, we are interested in 0,1,..
				//So, the '0' should be subtracted . i.e. the 49 value of '1' will be 49-48=1.
				//6 is used in indexing bcoz the input is of 6 bits
				int tid=omp_get_thread_num();
				row[tid] = 2 * int(x[tid * 6] - '0') + int(x[tid * 6 + 5] - '0'); 
				col[tid] = 8 * int(x[tid * 6 + 1] - '0') + 4 * int(x[tid * 6 + 2] - '0') + 2 * int(x[tid * 6 + 3] - '0') + int(x[tid * 6 + 4] - '0');
				val[tid] = sbox[tid][row[tid]][col[tid]];
				result[tid]= decimal_to_binary(val[tid]);
			}
		for(int i=0;i<8;i++) res+=result[i];

		// Step 2.4: P-Box Permutation 
		res = permutation(res, pbox, 32); 

		// Step 2.5: XOR of left and res 
		x = perform_xor(res, left); 
		left = x; 
		// Swap left and right in every rounds except the last round
		if (i != 15) { 
			swap(left, right); 
		} 
	}

	// Left and Right combined
	string combined = left + right; 

	// Step 3: Final Permutation to obtain 64bits cipher text
	string cipher = binary_to_string(permutation(combined, finalPermutation, 64)); 
	return cipher; 
} 

int main(int argc, char *argv[]){
	double t1=omp_get_wtime(); //starts the timer

	//Reading from the file and storing the plaintext in pt vector
	vector<string> pt;
	string c,temp;

	//creating a stream to read the plain text
	string file_name = argv[1];
	ifstream plain_text(file_name);

	//reading the plaintext file
	while(getline(plain_text,c)){
			temp+=c;
	}

	//if the length of plain text is not multiples of 8 then we're adding an extra space
	while(temp.length()%8!=0){
		temp+=" ";
	}

	//the plaintext is divided into strings of length '8' and each of these strings are stored in the vectors(blocks theoretically)
	//the length 8 is taken bcoz each alphabet when taken into ASCII in binary form it takes 8 bits
	//so 8 alphabets * 8 bits = 64 bit (which is perfect input for the DES algo)
	for(int i=0;i<temp.length();i=i+8){
		pt.push_back(temp.substr(i,8));
	}
	
	//closing the file
	plain_text.close();
	
	
	//key
	string key = "RAHUL3452KEDAR90";
	key = to_binary(key); //converting the key string to binary

	//Removing 8 bits from the key i.e. we're making the key without parity
	key = permutation(key, keyTransformation, 56);
	
	//Step 2.1: Key Transformation
	//Splitting 56 bit keys to left and right of 28 bits each
	string left = key.substr(0, 28); 
	string right = key.substr(28, 28); 
	vector<string> keys; // Declaring vector for storing keys of 16 rounds
	for (int i = 0; i < 16; i++) { 
		//Left Shift and Right Shift done to the respective left and right keys in each round
		left = shift_left(left, shiftsMatrix[i]); 
		right = shift_left(right, shiftsMatrix[i]); 
		string combinedkey = left + right; 
		// Key Compression : Converting 56 bit key to 48 bit combined key
		string RoundKey = permutation(combinedkey, keyCompresssion, 48); 
		keys.push_back(RoundKey); 
	}
	
	//Encryption
	vector<string> ciphertext;
	string cipher[pt.size()];
	cout<< "-------------------------------------" <<endl;
	cout << "Encrypting the given Plain Text"<<endl;

	//enable nested parallelism
	omp_set_nested(1);
	#pragma omp parallel for schedule(guided) num_threads(8)
	for(int i=0;i<pt.size();i++){
		cipher[i] = encryption(pt[i],keys);
	}
	for(int i=0;i<pt.size();i++){
		ciphertext.push_back(cipher[i]);
	}

	//Writing Cipher Text to File
	ofstream writeObj;
	remove("encrypted.txt");
	writeObj.open("encrypted.txt",ios::app);
	int j=0;
	for(j=0;j<ciphertext.size()-1;j++){	
		writeObj << ciphertext[j];
	}
	writeObj<<ciphertext[j+1];
	writeObj.close(); 
	cout<<"Encryption of Plain Text is Completed!!"<<endl<<endl;
	
	//Decryption
	//so we'll basically reverse the round keys and execute the encryption process to get Plain Text
	//a built-in function to simply reverse the keys inside the vector
	reverse(keys.begin(),keys.end()); 
	string decrypted;
	string text[ciphertext.size()];
	cout<< "-------------------------------------" <<endl;
	cout << "Decrypting the Encrypted Text"<<endl;

	//enable nested parallelism
	omp_set_nested(1);
	#pragma omp parallel for schedule(guided) num_threads(8)
	for(int i=0;i<ciphertext.size();i++){
		text[i] = encryption(ciphertext[i],keys);
	}
	for(int i=0;i<ciphertext.size();i++){
		decrypted+=text[i];
	}
	double t2=omp_get_wtime(); //end the timer

	//writing the decrypted text to a file
	ofstream decryp_obj("decrypted.txt");
	decryp_obj << decrypted;

	cout << "Decryption of Encrypted Text is Completed!!"<<endl;
	//cout << "\nThe Decrypted Plain Text:\n " << decrypted << endl;
	cout <<endl<<"Time taken for both Encryption and Decryption Process = "<<(t2-t1)<<"s"<<endl;  
}
