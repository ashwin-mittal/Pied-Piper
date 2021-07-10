#include "huffman.h"
#include "minHeap.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

int main(int argc, char *argv[])
{
	// Handling Wrong or Help arguments

	if (argc == 1)
	{
		printf("Use -h or --help flag for help.\n");
		return 1;
	}

	else if (argc == 2)
	{
		if (strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0)
		{
			printf("Compressor text files using static Huffman algorithm\n\n");
			printf("Usage: %s <path_to_input_file> <path_to_the_output_file>\n", argv[0]);
			return 0;
		}
		else
		{
			printf("Use -h or --help flag for help.\n");
			return 1;
		}
	}

	else if (argc > 3)
	{
		printf("Too many arguments! I don't know what to do..\n");
		printf("Use -h or --help flag for help.\n");
		return 1;
	}

	// Now, the actual implementation will begin.

	char *inputPath = argv[1];
	char *outputPath = argv[2];

	printf("Input file given: %s\n", inputPath);
	printf("\nReading from input file...\n");

	FILE *inputFile;

	inputFile = fopen(inputPath, "r");

	if (inputFile == NULL)
	{
		printf("Error: The input file could not be read.\n");
		return 1;
	}
	else
	{
		for (int charCode = 0; charCode < 256; charCode++)
			charFrequencies[charCode] = 0LL;

		printf("Input file successfully opened.\n");
		printf("Generating frequency table...\n");

		char c = fgetc(inputFile);
		long long int totalFreq = 0;
		fseek(inputFile, 0, SEEK_END);
		long long length = ftell(inputFile), temp = length;
		rewind(inputFile);

		while (temp--)
		{
			charFrequencies[charToInt(c)] += 1;
			totalFreq++;
			c = fgetc(inputFile);
		}

		printf("Generating codes...\n");

		for (int index = 0; index < 257; index++)
			minHeap[index] = NULL;

		for (int i = 0; i < 256; i++)
		{
			if (charFrequencies[i] > 0)
			{
				Node *node = newNode((char)i, charFrequencies[i], true);
				insertNode(node);
			}
		}

		while (minHeapSize > 1)
		{
			Node *smallestNode = getMinimumNode();
			removeMinimumNode();
			Node *nextSmallestNode = getMinimumNode();
			removeMinimumNode();

			/*
			*	Combined nodes have @ as the character.
			*	Original nodes can be identified by isLeafNode.
			*/

			Node *combinedNode = newNode('@', (smallestNode->frequency + nextSmallestNode->frequency), false);
			combinedNode->leftChild = smallestNode;
			combinedNode->rightChild = nextSmallestNode;
			insertNode(combinedNode);
		}

		fclose(inputFile);

		for (int i = 0; i < 256; i++)
		{
			codes[i] = NULL;
		}

		Node *head = minHeap[1];
		char startCode[] = "";
		formCodes(head, startCode);
		printf("Codes generated successfully\n");
		printf("Generating Output...\n");

		int countOfCodes = 0;
		long long countOfExcessBits = 0;
		long long countCharsToBeRead;

		for (int i = 0; i < 256; i++)
		{
			if (codes[i] != NULL)
			{
				countOfCodes++;
				countCharsToBeRead += (charFrequencies[i] * (long long)strlen(codes[i]->code));
				countOfExcessBits += (charFrequencies[i] * (long long)strlen(codes[i]->code));
				countOfExcessBits %= 8LL;
			}
		}

		int countOfBitsToIgnore = 8 - (int)countOfExcessBits;
		countCharsToBeRead = (countCharsToBeRead + countOfBitsToIgnore) / 8LL;

		FILE *outputFile;

		outputFile = fopen(outputPath, "wb");

		if (outputFile == NULL)
		{
			printf("The program was not able to open the output file...\n");
			return 1;
		}
		else
		{

			printf("Writing out file header...\n");

			fwrite(&countCharsToBeRead, sizeof(long long), 1, outputFile);
			fwrite(&countOfCodes, sizeof(int), 1, outputFile);
			fwrite(&countOfBitsToIgnore, sizeof(int), 1, outputFile);
			for (int i = 0; i < 256; i++)
			{
				if (codes[i] != NULL)
				{
					writeCode(codes[i], outputFile);
				}
			}

			inputFile = fopen(inputPath, "r");

			if (inputFile == NULL)
			{
				printf("The program ran into an error!");
				return 1;
			}
			else
			{
				printf("Writing out compressed file...\n");
				c = fgetc(inputFile);
				temp = length;
				while (temp--)
				{
					int codeLength = strlen((codes[charToInt(c)]->code));
					for (int i = 0; i < codeLength; i++)
					{
						char bit = codes[charToInt(c)]->code[i];
						if (bit == '1')
							writeBitToOutputFile(1, outputFile);
						else
							writeBitToOutputFile(0, outputFile);
					}
					c = fgetc(inputFile);
				}
				fclose(inputFile);

				for (int i = 0; i < countOfBitsToIgnore; i++)
				{
					writeBitToOutputFile(0, outputFile);
				}
			}
			fclose(outputFile);
		}
	}
	printf("Successfully compressed!\n");
	printf("Output File: %s\n", outputPath);
	printf("Good Bye.\n");
	return 0;
}