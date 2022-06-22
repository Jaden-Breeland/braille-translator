/**
 * Name: Jaden Breeland
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define MAX_CHAR (30)

#define LEFT "1 0 " //used to contrsuct braille letters one row at a time
#define RIGHT "0 1 "
#define BOTH "1 1 "
#define NEITHER "0 0 "
#define WLETTER "0 1 1 1 0 1 "
#define SPACE "0 0 0 0 0 0 "


FILE* in_file;
FILE* out_file;

/*This method will write segments of braille letters to the file. will write the top row, the middle, then the bottom row of letters
 
 @param braille_line: a string of braille bits that represent only wither the top, middle or bottom bits.
 
 @return 1 on success, -1 if an error occurs*/
int write_braille_to_file(char *braille_line)
{
  
  if(fwrite(braille_line, sizeof(char), strlen(braille_line), out_file) < 0){
    return -1;//failure writing
  }
    return 1;
}


//helper method to intiliaze a char array to be all null values
//accepts a char * pointer to  block of allocated but not intilaized memory
char *  initializeCharArray(char * pointer, int numBytes){
  int counter = 0;
  char * placeHolder = pointer;
  while(counter < numBytes){
    *pointer = 0;
    pointer ++;
    counter ++;
  }
  pointer = placeHolder;
  return pointer;
}


/*This method translates an ASCII char to a braille chararacter in linear fasion(a Brailler letter):
 0 3   =     0 3 1 4 2 5
 1 4
 2 5
 
 @param letter: the ASCII char to be translated.
 
 @return an char* that represents a Braille char*/
char * ASCII_to_braille(char letter)
{
  //this function builds the letter each row at a time. constants LEFT, RIGHT, BOTH, and NEITHER are string bit values that are used to construct the letter.
  char *braille = malloc(sizeof(char) * 13);//create space for the braille letter
  braille = initializeCharArray(braille, 13);
  int charVal = 0;
  charVal = letter - 'a';//charval is now on  scale from 0-25 representing a-z
  if(letter == 'w'){//w is exception
    strcat(braille, WLETTER);
    return braille;
  }
  else if(letter == ' '){
    strcat(braille, SPACE);
    return braille;
 }
  else if(charVal < 0 || charVal > 25){
    return braille;
  }


  int row = charVal / 10;//find the letter's position in the braille letter chart
  int col = charVal % 10;
  //populates the top two bits of the braille letter
  if(col == 0 || col == 1 || col == 4 || col == 7){//checks which column the letter is and adds bits accordingly
    strcat(braille, LEFT);
  }
  else if(col == 8 || col == 9){
    strcat(braille, RIGHT);
  }
  else{
    strcat(braille, BOTH);
  }
  
  //middle row of bits
  if(col == 1 || col == 5 || col == 8){
    strcat(braille, LEFT);
  }
  else if(col == 3 || col == 4){
    strcat(braille, RIGHT);
  }
  else if(col == 6 || col == 7 || col == 9){
    strcat(braille, BOTH);
  } 
  else{
    strcat(braille, NEITHER);
  }

  
  //final row of bits
  if(row == 0){
    strcat(braille, NEITHER);
  }
  else if(row == 1){
    strcat(braille, LEFT);
  }
  else{
    strcat(braille, BOTH);
  }
  return braille;
}

/*This method reads from in_file up to MAX_CHAR per call to fread(), then builds an 
 array of Braille letters by calling ASCII_to_braille(). It will write the Braille
 to out_file by calling write_braille_to_file().
 
 @return 1 on success, and -1 if an error occurs.*/
int translate_to_braille()
{
  char *input = malloc(sizeof(char) * 800);//holds inputfile
  char *inputBuf = malloc(sizeof(char) * MAX_CHAR);
  input = initializeCharArray(input, 800);
  inputBuf = initializeCharArray(inputBuf, MAX_CHAR);
  while(1){
    
    int charsRead = fread(inputBuf, sizeof(char), MAX_CHAR - 1, in_file);//read file andrecord how many bytes read
    if(charsRead < MAX_CHAR){//buffer is not full
      strncat(input, inputBuf,charsRead);//add buff to input and stop loop
      break;
    }
    else{
      strcat(input, inputBuf);
    }
  }
  if(strlen(input) == 0){
    printf("empty file");
    return -1;
  }
  free(inputBuf);




  char *brailleTop = malloc(sizeof(char) * 3200);//will remove the top row of each letter and store it here in order
  char *brailleMid = malloc(sizeof(char) * 3200);//stores middle rows of letters
  char *brailleBot = malloc(sizeof(char) * 3200);//stores final row of letters
  brailleTop = initializeCharArray(brailleTop, 3200);
  brailleMid = initializeCharArray(brailleMid, 3200);
  brailleBot = initializeCharArray(brailleBot, 3200);
  //parse each letter at a time, transalte it, then divide the letter to brailleTop, brailleMid, and brailleBot
  char **inputBeginning = malloc(8);
  *inputBeginning = input;//reset pointer later so it can be freed
  do{
    if(*input >= 65 && *input <= 90){//capital letter 
      *input += 32;//turn it into a lower case letter
      strcat(brailleTop, NEITHER);//add capital letter
      strcat(brailleMid, NEITHER);
      strcat(brailleBot, RIGHT);
      //add space buffer between characters
      strcat(brailleTop, " ");
      strcat(brailleMid, " ");
      strcat(brailleBot, " ");
    }
    else if(*input == ' '){//skip next if statement if char is a space
    }
    else if(*input < 97 || *input > 122){//not a letter
      input++;
      continue;
    }
    char *brailleChar = ASCII_to_braille(*input);//translate char
    strncat(brailleTop, brailleChar, 4);//divide letter to the appropiate strings
    strncat(brailleMid, brailleChar+4, 4);
    strncat(brailleBot, brailleChar+8, 4);
    strcat(brailleTop, " ");
    strcat(brailleMid, " ");
    strcat(brailleBot, " ");
    
    free(brailleChar);
    
    input++;//next letter
    
    
  }while(strlen(input) != 0);
  //replace the last space at the end of each string with a new line
  *(brailleTop + strlen(brailleTop) - 1) = '\n';
  *(brailleMid + strlen(brailleMid) - 1) = '\n';
  *(brailleBot + strlen(brailleBot) - 1) = '\n';

  write_braille_to_file(brailleTop);
  write_braille_to_file(brailleMid);
  write_braille_to_file(brailleBot);
  
  free(*inputBeginning);
  free(inputBeginning);
  free(brailleTop);
  free(brailleMid);
  free(brailleBot);
  
    return 1;
}







/*The program should validate that only one argument was supplied, and that it opens
 a valid file. The in_file will be translated from ASCII to Braille. It will create 
 the output file, and call translate_to_braille().

 The program should output the translation to a file named in_file.txt.output.
 e.g.: in_file: some_text.txt 
       out_file: some_text.txt.output
 
 @return 0 on success, and a 1 on error, exiting.*/
int main(int argc, char **argv)
{
  if(argc != 2){
    printf("Usage:\n./braille <src_file_name>");
    return 0;
  }
  //open file stream for input and create output file name
  in_file = fopen(argv[1], "r");
  char temp[50];
  strcpy(temp, argv[1]);
  char outputName[50];
  temp[strlen(temp)-4] = '\0';
  strcpy(outputName, temp);
  strcat(outputName, "_output.txt");

  out_file = fopen(outputName, "w");
  
  translate_to_braille();
  fclose(in_file);
  fclose(out_file);
  return 0;
}
