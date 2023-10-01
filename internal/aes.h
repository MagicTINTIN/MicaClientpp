#pragma once

//make sure that [size(ouput)>=size(text)*2+30] and that the key is 16 bytes long
void AES(unsigned char* text,unsigned char* key,unsigned char* output);
/**
 * @returns 1 if error
*/
int inv_AES(unsigned char* text,unsigned char* key,unsigned char* output);
/*
additionnal info :
the ouput could be much smaller but it would be unprintable (full of illegal characters)
only the 16th first char of the key will be used
both text MUST end with the \0 character, witch will not be counted
inv_AES will not work if the \0 is not correctly placed at the end of the the text to inverse
the output will have a \0 at the end
*/