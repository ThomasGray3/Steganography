//  C Coursework
//
//  Created by THOMAS GRAY on 08/02/2019.
//  Copyright Â© 2019 THOMAS GRAY. All rights reserved.
#include<stdio.h>
#include<stdlib.h>
#include <string.h>
#include <math.h>

// declare data structure to hold red, green and blue values
struct data {
    int r,g,b;
};

// declare structure for the PPM image
struct PPM {
    char P3[4];
    char comments;
    int width;
    int height;
    int max;
    struct data *Data;
};

// read in the PPM image and store it
struct PPM* GetPPM(FILE * fp){
    // declare variables
    int c;
    int r,g,b;
    int size;
    int i;
    i = 0;
    r = 0;
    g = 0;
    b = 0;
    char readChars[256] = {0};

    // allocates memory to the PPM
    struct PPM *im = (struct PPM *) calloc(1, sizeof(struct PPM));

    // checks PPM to make sure it is a P3
    fgets(im->P3, sizeof(im->P3), fp);
    // if the first and second character are not P and 3 then return null
    if((im->P3[0] != 'P') && (im->P3[0] != '3')){
        return NULL;
    }
    im->P3[2] = '\0';
    fgets(readChars, sizeof(readChars), fp);
    // read in comments
    while(readChars[0] == '#'){
        fgets(readChars, sizeof(readChars), fp);
    }
    // read in width and height
    sscanf(readChars, "%d %d", &im->width, &im->height);
    fgets(readChars, sizeof(readChars), fp);
    // read in max
    sscanf(readChars, "%d", &im->max);
    // allocate memory for the data structure
    im->Data = (struct data*)malloc(im->width * im->height * sizeof(struct data));
    // calculate number of pixels
    size = im->width * im->height;

    // loop to assign the numbers of the RGB to the data structure
    for (i=0;i<size;i++){
        fscanf(fp, "%d %d %d ", &r, &g, &b);
        im -> Data[i].r = (int) r;
        im -> Data[i].g = (int) g;
        im -> Data[i].b = (int) b;
    }
    // close file
    fclose(fp);
    return im;
}

// free up assigned memory
void FreePPM(struct PPM *im){
    free(im->Data);
    free(im);
}

// print the image in PPM format
void showPPM(struct PPM *im){
    int i = 0;
    // calculate the number of pixels
    int totalpixels = im->width * im->height;

    // print in PPM format with stored data
    printf("P3 %s\n", im->P3);
    printf("#CommentHere\n");
    printf("%d %d\n", im->width, im->height);
    printf("%d\n", im->max);

    // print all the RGB values in the correct format
    for(i = 0; i < totalpixels; ++i){
        printf("%d %d %d  ", im->Data[i].r, im->Data[i].g, im->Data[i].b);
        if((i+1)%im->width == 0 && i > 0){
          printf("\n");
        }
    }
}

// converts decimal to binary
long long convertToBinary(int ascii){
    long long binaryNumber = 0;
    int remainder, i = 1, step = 1;
    // loop until the number is 0
    while (ascii != 0){
        // calculate remainder, either 1 or 0
        remainder = ascii%2;
        // half the number before looping again
        ascii /= 2;
        binaryNumber += remainder*i;
        i *= 10;
    }
  return binaryNumber;
}

// convert binary to decimal
int convertToDecimal(long long n){
      int  num, decimalNumber = 0, base = 1, rem;
      num = n;
      // loop until the number is <= 0
      while (num > 0) {
          // decimal calculation
          rem = num % 10;
          decimalNumber = decimalNumber + rem * base;
          num = num / 10 ;
          base = base * 2;
      }
    return decimalNumber;
}

// encoding the secret message to the PPM
struct PPM * encode(struct PPM * im, char * message, unsigned int mSize, unsigned int secret){
    // variable declaration
    int k;
    int m = -1;
    int ascii[20];
    int i;
    int j;
    //mSize++;
    int sizeBinary = 7*mSize;
    int imageSize = im->width * im->height;
    int binArr[sizeBinary];
    long long binary[mSize];
    int finalBinArr[sizeBinary];


	// add to end of message so we can find it when decoding
    strcat(message, "/");
    //increase message size
    mSize = strlen(message);

    // turn secret message from characters to ascii
    for(i=0; i<mSize;++i){
        ascii[i] = (int) message[i];
    }
    // loop for the length of the message
    for(j=0; j<mSize; ++j){
        // returns the binary number and stores in array
        binary[j] = convertToBinary(ascii[j]);
    }

    j=0;
    // separates the binary number so that each bit is stored in its own index
    for(i=(mSize-1); i>=0;--i){
        while (binary[i] > 0) {
            // assign the individual bit to a new index
            binArr[j] = binary[i] % 10;
            j++;
            binary[i] /= 10;
        }
    }

    k=0;
    // reverse the array so it is the correct way round
    int count = sizeBinary;
    for (k = 0; k < sizeBinary; ++k) {
        // assign last index of the old array to the first of a new array
        finalBinArr[k] = binArr[count-1];
        count--;
    }

    // calculate the required amount of random numbers
    srand(secret);
    int sizeRand = sizeBinary/3;
    if(sizeBinary%3 != 0){
        ++sizeRand;
    }
    
    // calculate the random numbers based of the secret key
    int randomNumbers[sizeRand];
    for(i=0; i<sizeRand; ++i){
        randomNumbers[i] =rand() % (imageSize);

    }


    long long redPixBin;
    long long greenPixBin;
    long long bluePixBin;
    i=0;
    // loop for size of the message in bits
    for(j=0; j<sizeBinary; ++j){
    	// access the value of each colour stored in the pixel at index of the random number
        redPixBin = convertToBinary(im->Data[randomNumbers[i]].r);
        greenPixBin = convertToBinary(im->Data[randomNumbers[i]].g);
        bluePixBin = convertToBinary(im->Data[randomNumbers[i]].b);

		// calculation for adding 1 or 0 to the last bit
        if(finalBinArr[j] == 1){
            redPixBin = redPixBin/10*10 +1;
        }else{
              redPixBin = redPixBin/10*10;
        }
        if(finalBinArr[j+1] == 1){
            greenPixBin = greenPixBin/10*10 +1;
        }else{
              greenPixBin = greenPixBin/10*10;
        }
        if(finalBinArr[j+2] == 1){
            bluePixBin = bluePixBin/10*10 +1;
        }else{
              bluePixBin = bluePixBin/10*10;
        }
        j=j+2;

		// convert from binary back to decimal
        int newRedPixBin = convertToDecimal(redPixBin);
        int newGreenPixBin =convertToDecimal(greenPixBin);
        int newBluePixBin =convertToDecimal(bluePixBin);

		// store the updated pixel colours
        im->Data[randomNumbers[i]].r = newRedPixBin;
        im->Data[randomNumbers[i]].g = newGreenPixBin;
        im->Data[randomNumbers[i]].b = newBluePixBin;

        ++i;
    }
    // print the updated image to the new ppm file
    showPPM(im);
}

char * decode(struct PPM * im, unsigned int secret){

    int end = 0;
    //int imageSize = im->width * im->height;
    showPPM(im);
  //  srand(secret);
  /* while(end==0){
        printf("%d\n", im->width);
        printf("%d\n", im->height);
        printf("%d\n", secret);
      //  printf("%d\n", imageSize);
      /*  int randomNumber =rand() % imageSize;
        printf("%d\n", im->Data[randomNumber].r);
        printf("%d\n", im->Data[randomNumber].g);
        printf("%d\n", im->Data[randomNumber].b);
        long long redPixBin = convertToBinary(im->Data[randomNumber].r);
        long long greenPixBin = convertToBinary(im->Data[randomNumber].g);
        long long bluePixBin = convertToBinary(im->Data[randomNumber].b);
        printf("%lld\n", redPixBin );
        printf("%lld\n", greenPixBin );
        printf("%lld\n", bluePixBin );
  //  }*/
}





int main(int argc, char * argv[]){

	// variable declaration
    char fileName[40];
    char message[40];
    int mSize = 0;
    int temp;
    int secret;
    char term;
    int input;

    // check if the user wants to encode or decode
    int resE = strcmp(argv[1], "e");
    int resD = strcmp(argv[1], "d");
    
    // if argv = e then perfom encode
    if(resE==0){
        fprintf(stderr, "----Encode----\n");
		// open file
        FILE *fp = fopen(argv[2], "r");
		// error message if file doesnt exist
        if(fp==NULL){
            fprintf(stderr, "\nError: cannot open file");
            exit(1);
        }
		// create a structure for the image
        struct PPM *im = GetPPM(fp);
		// free space
        FreePPM(im);
		// receive message to encode
        fprintf(stderr, "Enter the message to encode:\n");
        scanf("%s", message);
        mSize = strlen(message);
        fprintf(stderr, "\n");
		// receive key 
        fprintf(stderr, "Enter key to allow decode:\n");
        if(scanf("%d%c", &temp, &term) != 2 || term != '\n'){
            fprintf(stderr, "Invalid input\n");
            exit(1);
        }else{
            secret = temp;
        }
    // begin to encode
    encode(im, message, mSize, secret);
    }

	// if argv = d begin decode
    if(resD == 0){
        fprintf(stderr, "----Decode----\n");
		
		// open file
        FILE *fp = fopen(argv[2], "r");
        if(fp==NULL){
            fprintf(stderr, "\nError: cannot open file");
            exit(1);
        }
		
		// create structure for the image
        struct PPM *im = GetPPM(fp);
        FreePPM(im);
		// receive key from user
        fprintf(stderr, "Enter the secret key:\n");
        scanf("%d", &secret);
		// begin decode
        decode(im, secret);

    }
  return 0;
}
