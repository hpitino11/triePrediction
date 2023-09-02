// Hanna Pitino
// Trie Predicition based on corpus input file

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "TriePrediction.h"

#define MAX_WORD_LENGTH 1023

TrieNode *createTrieNode(void)
{
	TrieNode *newNode = (TrieNode*) malloc(sizeof(TrieNode));
		newNode->count = 0;
		newNode->subtrie = NULL;

		// Sets each of the children of the node to NULL
		for (int i = 0; i < 26; i++) 
		{
			newNode->children[i] = NULL;
		}

	return newNode;
}

void insertString(TrieNode *root, char *str)
{
	// If either the roor or string is null then return
	if(root == NULL || str == NULL)
	{
		return;
	}
	// While str is not at the end then iterate through each child 
	// and create a node for each
	while (*str != '\0')
	{
		TrieNode **child = &root->children[*str - 'a'];

		if (*child == NULL)
		{
			*child = createTrieNode();
		}
		root = *child;
		str++;
	}
	// update count to represent the newly added string
	root->count++;
}

// helper function to traverse the trie to the node that corresponds to the prefix prev_word
TrieNode* traverseTrie(TrieNode *root, char *prev_word) 
{
    TrieNode *temp = root;
    int index = 0;
    int i = 0;

	// Loop through the prev word until the end of the string
    for (i = 0; prev_word[i] != '\0'; i++) 
    {
        index = prev_word[i] - 'a';
		// if the child does not exist then create a new node
        if (!temp->children[index]) 
        {
            temp->children[index] = createTrieNode();
        }
        temp = temp->children[index];
    }
    return temp;
}

// Function to insert a new word into the trie
void insertTrie(TrieNode *root, char *prev_word, char *new_word) 
{
	// Gets the node that is with the prefix of prev_word
    TrieNode *temp = traverseTrie(root, prev_word);

	// If there is no subtrie then create a new node
    if (!temp->subtrie) 
    {
        temp->subtrie = createTrieNode();
    }
	// Insert the new word into the current node's subtree
    insertString(temp->subtrie, new_word);
}

// loosely based on tries.c but has a lot more added to it to improve functionality with punctuation
TrieNode *buildTrie(char *filename) 
{
	FILE *fp = fopen(filename, "r");
	if (fp == NULL) 
	{
		return NULL;
	}
	// creates the root of the trie
	TrieNode *root = createTrieNode();
	if (root == NULL) 
	{
		// root is NULL we need to close the file and return NULL to avoid leaks
		fclose(fp);
		return NULL;
	}

	char word[1026];
	char prev_word[1026] = "";
	int prev_word_exists = 0;
	int length = 0;

	while (fscanf(fp, "%s", word) != EOF) 
	{
		// remove all of the punctuation 
		int i, j;
		char new_word[1026];
		for (i = 0, j = 0; word[i]; i++) 
		{
			if (isalpha(word[i])) 
			{
				new_word[j++] = tolower(word[i]);
			}
		}
		new_word[j] = '\0';

		// insert new_word into trie
		insertString(root, new_word);

		if (prev_word_exists) 
		{
			insertTrie(root, prev_word, new_word);
		}

		strcpy(prev_word, new_word);
		prev_word_exists = 1;

		length = strlen(word);
		if (word[length - 1] == '.' || word[length - 1] == '!' || word[length - 1] == '?') 
		{
			prev_word_exists = 0;
		}
	}
	fclose(fp);
	return root;
}

int containsWord(TrieNode *root, char *str)
{
	// if the root or str are NULL then return 0 without initializing anything
	if(root == NULL || str == NULL)
	{
		return 0;
	}

	int count = 0;
	int length = strlen(str);
	TrieNode *temp = root;

	// while it is not at the end of the sentence
	// move down to the child node corresponding to the current character
	while ((str[count] != '\0')) 
	{
		// if the current character does not have a child node in the trie, the while loop will exit 
		// and 'count' will be the length of the longest prefix of 'str' that is in the trie, meaning it will be
		// greater than 0
		if( temp->children[str[count] - 'a'] != NULL)
		{
			temp = temp->children[str[count] - 'a'];
			count++;
		}
		else
		{
			return 0;
		}
	}
	if (temp->count > 0)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

// helper function to destroy trie to keep things cleaner and easier
void destroyTrieHelper(TrieNode *node) 
{
	// if NULL return
	if (node == NULL) 
	{
		return;
	}
	// each child will be recursively destroyed
	for (int i = 0; i < 26; i++) 
	{
		destroyTrieHelper(node->children[i]);
	}

	// recursively destroy each subtrie, which will call the helper
	destroyTrie(node->subtrie);
	free(node);
}

TrieNode *destroyTrie(TrieNode *root) 
{
	destroyTrieHelper(root);
	return NULL;
}

void getMostFrequentWordHelper(TrieNode *node, char *word, char *max_word, int *max_count) 
{
	if (node == NULL) 
	{
		return;
	}

	int i = 0;

	// if the count of the current node is > then the max_count
	// update the max_count to the current node's count
	// and copy the word into the max_word
	if (node->count > *max_count) 
	{
		*max_count = node->count;
		strcpy(max_word, word);
	}
	else if (node->count == *max_count && strcmp(word, max_word) < 0)
	{
		strcpy(max_word, word);
	}

	for (i = 0; i < 26; i++) 
	{
		if (node->children[i] != NULL) 
		{
			// add the letter of the child to the end of the current word and update the null terminator.
			word[strlen(word)] = i + 'a';
			word[strlen(word) + 1] = '\0';
			getMostFrequentWordHelper(node->children[i], word, max_word, max_count);

			// after the recursion, removes the last letter of the current word and updates the null terminator.
			word[strlen(word) - 1] = '\0';
		}
	}
}

void getMostFrequentWord(TrieNode *root, char *str) 
{
	// if the root is NULL then set the str empty and return
	if (root == NULL) 
	{
		str[0] = '\0';
		return;
	}

	char word[MAX_CHARACTERS_PER_WORD];
	char max_word[MAX_CHARACTERS_PER_WORD];

	// set the last character to a null terminator
	int max_count = 0;
	word[0] = '\0';
	word[MAX_CHARACTERS_PER_WORD - 1] = '\0';
	getMostFrequentWordHelper(root, word, max_word, &max_count);

	strcpy(str, max_word);
}


// based on tries.c insert function 
TrieNode *getNode(TrieNode *root, char *str)
	{
	if (root == NULL || str == NULL) 
	{
		return NULL;
	}

	TrieNode *curr = root;
	int index;
	int j = 0;

	// since the trie is case sensitive, added tolower to still find the word within
	// the trie in case there is a casing issue
	for (int i = 0; str[i] != '\0'; i++) 
	{
		index = tolower(str[i]) - 'a';
		if(index < 0 || index >= 26)
		{
			return NULL;
		}

		if (curr->children[index] == NULL) 
		{
			// string is not found in trie
			return NULL; 
		}
		curr = curr->children[index];
		}

		// string is not found in trie
		if (curr->count == 0) 
		{
			return NULL; 
		}
	return curr;
}



int prefixCount(TrieNode *root, char *str) 
{
	TrieNode *current = root;
	int i = 0;
	int count = 0;

	while (str[i] != '\0') 
	{
		int index = str[i] - 'a';

		// if the child node of the character is NULL
		// there is no word with that prefix in it so return 0
		if (current->children[index] == NULL) 
		{
			return 0;
		}
		// move the current to the next child node
		current = current->children[index];
		i++;
	}
	if (current->count > 0) 
	{
		count += current->count;
	}

	// if there is a child that exists then recursively call the child 
	// and increment count
	for (int j = 0; j < 26; j++) 
	{
		if (current->children[j] != NULL) 
		{
			count += prefixCount(current->children[j], "");
		}
	}

	return count;
}

// Helper function called by printTrie(). (Credit: Dr. S.)
void printTrieHelper(TrieNode *root, char *buffer, int k)
{
	int i;

	if (root == NULL)
	{
		return;
	}

	if (root->count > 0)
	{
		printf("%s (%d)\n", buffer, root->count);
	}

	buffer[k + 1] = '\0';

	for (i = 0; i < 26; i++)
	{
		buffer[k] = 'a' + i;
		printTrieHelper(root->children[i], buffer, k + 1);
	}

	buffer[k] = '\0';
}

// If printing a subtrie, the second parameter should be 1; otherwise, if
// printing the main trie, the second parameter should be 0. (Credit: Dr. S.)
void printTrie(TrieNode *root, int useSubtrieFormatting)
{
	char buffer[1026];

	if (useSubtrieFormatting)
	{
		strcpy(buffer, "- ");
		printTrieHelper(root, buffer, 2);
	}
	else
	{
		strcpy(buffer, "");
		printTrieHelper(root, buffer, 0);
	}
}

TrieNode* findPrefix(TrieNode* root, char* prefix)
{
	TrieNode* node = root;
	int i = 0;
	int index = 0;
	int length = strlen(prefix);

	for (i = 0; i < length; i++)
	{
		// if the character is not lower case then this sets it to lowercase
		if(islower(prefix[i]) == 0)
		{
			index = tolower(prefix[i]) - 'a';
		}
		else
		{
			index = prefix[i] - 'a';
		}

		if(node->children[index] == NULL)
		{
			return NULL;
		}
		// move along to the next node
		node = node->children[index];
	}
	return node;
}

char* toLowerCase(char* word) 
{
	// sets every character in the word to lowercase
	for (int i = 0; word[i]; i++) 
	{
		word[i] = tolower(word[i]);
	}
	return word;
}

int processInputFile(TrieNode* root, char* filename) 
{
	FILE* fp;
	char buffer[1026];

	if ((fp = fopen(filename, "r")) == NULL) 
	{
		// returns 1 if the input file does not exist
		return 1;
	}

	while (fscanf(fp, "%s", buffer) != EOF) 
	{
		switch (buffer[0]) 
		{
			// print the trie
			case '!':
				printTrie(root, 0);
				break;

			// text predicition with no punctuation
			case '@':
				fscanf(fp, "%s", buffer);
				int n;
				fscanf(fp, "%d", &n);
				TrieNode* node = getNode(root, buffer);
				if (node == NULL) 
				{
					printf("%s\n", buffer);
					break;
				}
				char prevWord[1026] = "";
				printf("%s", buffer);
				for (int i = 1; i <= n; i++) 
				{
					if (node->subtrie == NULL) 
					{
						break;
					}
					// word works almost as a buffer
					char word[1026] = "";
					getMostFrequentWord(node->subtrie, word);

					// if returns null then the word was not present and something is wrong 
					// this is more so for my own testing
					if(getMostFrequentWord == NULL)
					{
						printf("ERROR. Input word not present in the trie.\n");
					}
					if (i == n) 
					{
						printf(" %s", word); // print the original word
					} 
					else 
					{
						printf(" %s", toLowerCase(word)); // print the lowercase version of the word
					}
					strcpy(prevWord, word);
					node = getNode(root, prevWord);
				}
				printf("\n");
				break;

			default:
				// prints the string and its subtries
				printf("%s\n", buffer);
				TrieNode* temp = findPrefix(root, buffer);
				// if prefix is NULL the string is invalid
				if (temp == NULL) 
				{
					printf("(INVALID STRING)\n");
				} 
				// again, invalid if it is not found
				else if (temp->count == 0) 
				{
					printf("(INVALID STRING)\n");
				} 
				// if the subtries are NULL that means it is empty
				else if (temp->subtrie == NULL) 
				{
					printf("(EMPTY)\n");
				} 
				else 
				{
					printTrie(temp->subtrie, 1);
				}
				break;
		}
	}
	fclose(fp);

	// returns 0 if the input file does exist
	return 0; 
}


int newNodeCount(TrieNode *root, char *str)
{
	// if the first character is a NULL terminator then the string is empty
	// meaning it needs no new nodes (return 0)
	if (str == NULL || str[0] == '\0')
	{
		return 0;
	}

	int count = 0;
	char c;
	int index;
	TrieNode *curr = root;

	if (root == NULL) 
	{
		count++;
		curr = createTrieNode();
	}

	// Iterate through each character in str
	for (int i = 0; str[i] != '\0'; i++) 
	{
		// ignore any non-alphabetic characters
		if (!isalpha(str[i])) 
		{
			continue;
		}

		// convert to lowercase for case-insensitive comparison
		c = tolower(str[i]);

		// makes sure that any character that is not in the alphabet
		// (specifically NULL terminators) are not going to be found
		if(!(c >= 'a' && (c <= 'z')))
		{
			return 0;
		}

		// the index that is in the trie since we can't do indexes of a character
		index = c - 'a';

		if (index < 0 || index >= 26) 
		{
			continue;
		}
		
		// if the current node doesn't have a child for this character, increment count
		// move to the child node for this character
		if (curr->children[index] == NULL) 
		{
			count++;
			curr->children[index] = createTrieNode();
		}
		
		// resets the curr to the next one
		curr = curr->children[index];
	}
	return count;
}

double difficultyRating(void)
{
	return 4.5;
}

double hoursSpent(void)
{
	return 20.0;
}

int main(int argc, char **argv) 
{
	int count = 0;
	int process;
	if (argc > 3 || argc < 3) 
	{
		printf("Too many/little arguments passed.\n");
		return 1;
	}

	TrieNode* root = buildTrie(argv[1]);
	process = processInputFile(root , argv[2]);
	root = destroyTrie(root);
	
	return 0;
}
