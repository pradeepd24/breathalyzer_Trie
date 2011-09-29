/* This uses Levenshtein distance as the Edit distance and uses a Trie for optimizing in searching the closest word to the query word */
/* This link explains about the algorithm used here. http://stevehanov.ca/blog/index.php?id=114   */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//Trie Node
struct Trie_Node {
	char c;
	int is_word;
	struct Trie_Node *children[26];
};

struct Trie_Node *root;
int MIN_COST, TOTAL_MIN_COST = 0;

void InitialiseTrieNode(struct Trie_Node **node, char c) {
   
   int i = 0;
   (*node)->c = c;
   (*node)->is_word = 0;
   for(i = 0; i < 26; i++)
     	(*node)->children[i] = NULL;

}

//Inserting str in the trie, it creates the trie nodes needed to insert the string, str
void InsertTrie(struct Trie_Node **node, char *str) {

   if(str[0] == '\0') 
   	return;

   if(*node == NULL) {
   	*node = (struct Trie_Node *)malloc(sizeof(struct Trie_Node));
   	InitialiseTrieNode(node, str[0]);
   }

   if(str[1] == '\0') {
   	(*node)->is_word = 1;
        return;
   }
   
   InsertTrie(&((*node)->children[str[1]-'a']), &str[1]);
   return;
}

//Builds the trie by inserting all words in the trie. All upper case aplhabets are converted to lower case before inserting.
void BuildTrie(char *dictionary) {

   FILE *fp;
   char word[25];
   int i = 1;
   fp = fopen(dictionary, "r");
   word[0] = '$';

   while(fscanf(fp,"%s",&word[1]) != EOF) {
	i = 1;
	while(word[i] != '\0') {
        	if(word[i] >= 'A' && word[i] <= 'Z')
           		word[i] = word[i] + 32;
		i++;
	}
   	InsertTrie(&root, word);
   }

   fclose(fp);
}


int minimum(int a, int b, int c) {

   if(a <= b && a <= c)
   	return a;
   if(b <= a && b <= c)
   	return b;
   return c;
}

// Recursively searches the trie for queryword. previous row contains the edit distance for the string upto this trie node.
void SearchRecursive(struct Trie_Node *node, char *queryword, int *previous_row, int length) {

   int current_row[length+1];
   current_row[0] = previous_row[0]+1;
   int min_current_row = current_row[0];
   int ins_cost, del_cost, repl_cost;
   int i;
   
   // computes edit distance for this node.
   for(i = 1; i <= length; i++) {
   	ins_cost = current_row[i-1]+1;
        del_cost = previous_row[i]+1;
   
	if(node->c == queryword[i-1])
        	repl_cost = previous_row[i-1];
        else
        	repl_cost = previous_row[i-1]+1;

        current_row[i] = minimum(ins_cost, del_cost, repl_cost);
        if(min_current_row > current_row[i])
  		min_current_row = current_row[i];
    }

    if(node->is_word == 1 && current_row[length] < MIN_COST) {
   	 MIN_COST = current_row[length];
    }
    
    // only if the min of current row is < MIN_COST, search further. This prunes the search.
    if(min_current_row < MIN_COST) {
    	for(i = 0; i <= 25; i++) {
        	if(node->children[i] != NULL)
           		SearchRecursive(node->children[i], queryword, current_row, length);
        }
    }
}

// Search the trie from the root node
void SearchTrie(char *queryword) {

    int length = strlen(queryword);
    int current_row[length+1];
    int i = 0;
    for(i = 0; i <= length; i++) {
    	current_row[i] = i;
    }

    for(i = 0; i <= 25; i++) {
    	if(root->children[i] != NULL)
        	SearchRecursive(root->children[i], queryword, current_row, length);
    }
}


int main(int argc, char *argv[]) {

    BuildTrie("/var/tmp/tw106.txt");
    FILE *fp;
    fp = fopen(argv[1], "r");

    char queryword[25];
    while(fscanf(fp,"%s",queryword) != EOF) {
	MIN_COST = strlen(queryword); //in the worst case MIN_COST = strlen(queryword)
        SearchTrie(queryword);
        TOTAL_MIN_COST = TOTAL_MIN_COST + MIN_COST;
    }

    printf("%d\n",TOTAL_MIN_COST);
    fclose(fp);
    return 1;
}
