#include <stdio.h>
#include <malloc.h>
#include <stdbool.h>
#include <string.h>
#define MAX_CHARACTERS  64
#define FILTER 5
#define ARR_LENGHT 5
#define THRESHOLD 5000
#define NODES_PER_BLOCK 20000

typedef struct node_t {
    char *key;
    struct node_t *father,*s_lef,*s_right;
    bool flag;
}node_t;

typedef struct Node {
    char *word;
    bool isPresent;
    struct Node *next;
} Node;
typedef struct CharacterInfo {
    char character;
    int isPresent;
    int notPosition[ARR_LENGHT];
    int exactPositions[ARR_LENGHT];
    int minNumberOccurrences;
    int exactNumberOccurrences;
} CharacterInfo;
CharacterInfo characterArray[MAX_CHARACTERS];
CharacterInfo filterArray[FILTER];
Node *createListNode(char *word,int dim) {
    Node *newNode = (Node*) malloc(sizeof(Node)+dim+1);
    newNode->word = (char*) (newNode+1);
    strncpy(newNode->word,word,dim);
    newNode->word[dim] = '\0';
    newNode->isPresent = true;
    newNode->next = NULL;
    return newNode;
}
Node *insertInOrder(Node *head, char *word,int dim) {
    if (head == NULL) {
        return createListNode(word,dim);
    }

    Node *current = head;
    Node *previous = NULL;

    while (current != NULL) {
        int comparison = strcmp(word, current->word);
        if (comparison < 0) {
            break;
        }
        previous = current;
        current = current->next;
    }

    Node *newNode = createListNode(word, dim);

    if (previous == NULL) {
        newNode->next = head;
        head = newNode;
    } else {
        newNode->next = current;
        previous->next = newNode;
    }

    return head;
}

Node *insertAtTail(Node *head, char *word,int dim) {
    if (head == NULL) {
        return createListNode(word,dim);
    }

    Node *current = head;
    while (current->next != NULL) {
        current = current->next;
    }

    current->next = createListNode(word,dim);
    return head;
}


void printList(Node *head) {
    Node *current = head;
    while (current != NULL) {
        printf("%s\n", current->word);
        current = current->next;
    }
}

node_t** nodes = NULL;
int node_count = 0;
int node_capacity = 0;

node_t* createNode(char *word, int dim) {
    if (node_count == node_capacity) {
        node_capacity = (node_capacity == 0) ? NODES_PER_BLOCK : node_capacity+NODES_PER_BLOCK;
        int size = sizeof(node_t) + dim + 1;
        nodes = malloc( sizeof(node_t*) * node_capacity + size * node_capacity);
        char *data = (char*)(nodes + node_capacity);
        for (int i = node_count; i < node_capacity; i++) {
            nodes[i] = (node_t*)data;
            data += size;
        }
    }
    node_t* new_node = nodes[node_count++];
    new_node->key = (char*)(new_node + 1);
    strncpy(new_node->key, word, dim);
    new_node->key[dim] = '\0';
    new_node->flag = true;
    new_node->father = NULL;
    new_node->s_lef = NULL;
    new_node->s_right = NULL;
    return new_node;
}

node_t* createTree(int dim){
node_t *n = malloc(sizeof (node_t)+dim+1);
 n->key = (char*)(n+1);
  n->flag = true;
  n->father = NULL;
 n->s_lef = NULL;
 n->s_right = NULL;
 return n;
}

void insertNode (node_t *tree,node_t *node){
    node_t *y ;
    node_t *x ;
    y = NULL;
    x = tree->father;
    while (x != NULL){
        y = x;
        if (strcmp(node->key , x->key) < 0)
            x = x->s_lef;
        else x = x->s_right;
    }
    node->father = y;
    if (y == NULL)
        tree->father = node;
    else if (strcmp(node->key , y->key) < 0)
        y->s_lef = node;
    else y->s_right = node;
}
int max(int a,int b){
    int c = a;
    int d = b;
    if (c>d) return c;
    else return d;
}
void printkey(node_t *tree){
    if (tree != NULL){
        printkey(tree->s_lef);
        if (tree->flag == true) {
            printf("%s\n", tree->key);
        }
        printkey(tree->s_right);
    }
}

bool WordSearch(node_t* tree,char* word){
    if (tree == NULL) return false;
    int cmp = strcmp(word,tree->key);
    if (cmp == 0) return true;
    if (cmp < 0 ){
        return WordSearch(tree->s_lef,word);
    }
    else {
        return WordSearch(tree->s_right,word);
    }

}
void deallocate(node_t*tree){
    if (tree == NULL) return;
    deallocate(tree->s_lef);
    deallocate(tree->s_right);
    free(tree);

}
void reinitializeTree(node_t*tree){
    if(tree!=NULL){
        reinitializeTree(tree->s_lef);
        tree->flag = true;
        reinitializeTree(tree->s_right);
    }
}
void initializeFilter() {
    for (int i = 0; i < FILTER; i++) {

        filterArray[i].character = '\0';
        filterArray[i].isPresent = 0;
        for (int j = 0; j < ARR_LENGHT; j++) {
            filterArray[i].exactPositions[j] = -1;
            filterArray[i].notPosition[j] = -1;
        }
        filterArray[i].exactNumberOccurrences = -1;
        filterArray[i].minNumberOccurrences = 1;

    }
}
void initializeCharacterArray() {
    for (int i = 0; i < MAX_CHARACTERS; i++) {

        characterArray[i].character = '\0';
        characterArray[i].isPresent = 0;
        for (int j = 0; j < ARR_LENGHT; j++) {
            characterArray[i].exactPositions[j] = -1;
            characterArray[i].notPosition[j] = -1;
        }
        characterArray[i].exactNumberOccurrences = -1;
        characterArray[i].minNumberOccurrences = 1;

    }
}
int findCharacterInArray(char c) {
    for (int i = 0; i < MAX_CHARACTERS; i++) {
        if (characterArray[i].character == c) {
            return i;
        }
        if(characterArray[i].character=='\0') break;
    }
    return -1;
}void updateFilter(CharacterInfo info) {

    // character is not present in the array
    for (int i = 0; i < FILTER; i++) {
        if (filterArray[i].character == '\0') {
            if (info.isPresent == 0) {
                filterArray[i].character = info.character;
                filterArray[i].isPresent = info.isPresent;
                break;
            }
            filterArray[i].character = info.character;
            filterArray[i].isPresent = info.isPresent;
            if (info.exactNumberOccurrences != -1)
                filterArray[i].exactNumberOccurrences = info.exactNumberOccurrences;
            if (info.minNumberOccurrences != 1) {
                    filterArray[i].minNumberOccurrences = info.minNumberOccurrences;

                }
            if (info.exactPositions[0] != -1) {
                for (int j = 0; j < ARR_LENGHT; j++) {
                    if (filterArray[i].exactPositions[j] == -1) {
                        filterArray[i].exactPositions[j] = info.exactPositions[0];
                        break;
                    }
                }
            }
            if (info.notPosition[0] != -1) {
                for (int j = 0; j < ARR_LENGHT; j++) {
                    if (filterArray[i].notPosition[j] == -1) {
                        filterArray[i].notPosition[j] = info.notPosition[0];
                        break;
                    }
                }
            }
            break;
        }
    }
}


void updateCharacterArray(CharacterInfo info) {
    int charIndex = findCharacterInArray(info.character);
    if (charIndex == -1) {
        // character is not present in the array
        for (int i = 0; i < MAX_CHARACTERS; i++) {
            if (characterArray[i].character == '\0') {
                characterArray[i].character = info.character;
                if (info.isPresent == 0) { break; }
                characterArray[i].isPresent = info.isPresent;
                if(info.exactNumberOccurrences != -1) characterArray[i].exactNumberOccurrences = info.exactNumberOccurrences;
                if(info.minNumberOccurrences !=1) characterArray[i].minNumberOccurrences = info.minNumberOccurrences;
                if(info.exactPositions[0] != -1) {
                    for (int j = 0; j < ARR_LENGHT; j++) {
                        if (characterArray[i].exactPositions[j] == -1){
                            characterArray[i].exactPositions[j] = info.exactPositions[0];
                            break;
                        }
                    }
                }
                if(info.notPosition[0] != -1) {
                    for (int j = 0; j < ARR_LENGHT; j++) {
                        if (characterArray[i].notPosition[j] == -1){
                            characterArray[i].notPosition[j] = info.notPosition[0];
                            break;
                        }
                    }
                }
                break;
            }
        }
    } else {
        if(characterArray[charIndex].isPresent != 0) {
            // character is already present in the array
            if(info.exactNumberOccurrences != -1) characterArray[charIndex].exactNumberOccurrences = info.exactNumberOccurrences;
            if(info.minNumberOccurrences !=1 && info.minNumberOccurrences >= characterArray[charIndex].minNumberOccurrences) characterArray[charIndex].minNumberOccurrences = info.minNumberOccurrences;
            if(info.exactPositions[0] != -1) {
                for (int j = 0; j < ARR_LENGHT; j++) {
                    if (characterArray[charIndex].exactPositions[j] ==info.exactPositions[0] ) break;
                    else if (characterArray[charIndex].exactPositions[j] == -1 ) {
                        characterArray[charIndex].exactPositions[j] = info.exactPositions[0];
                        break;
                    }

                }
            }
            if(info.notPosition[0] != -1) {
                for (int j = 0; j < ARR_LENGHT; j++) {
                    if (characterArray[charIndex].notPosition[j] ==info.notPosition[0] ) break;
                    else if (characterArray[charIndex].notPosition[j] == -1 ) {
                        characterArray[charIndex].notPosition[j] = info.notPosition[0];
                        break;
                    }
                }
            }
        }
    }
}
void WordConfrontation(char* reference,char*word){
    int i,j,g,atLeast = 1,nWordInReference=0,nCorrectInWord=0,nWrongInWord=0;
    int dim = strlen(word);
    CharacterInfo info;
    char *ris = malloc(dim+1*sizeof (char));
    ris[dim]='\0';

    for(i=0;i<dim;i++ ) {
        nWrongInWord=0;
        nCorrectInWord=0;
        nWordInReference=0;
        if (word[i] == reference[i]) {
            ris[i] =  '+';
            info.exactPositions[0]=i;
            info.notPosition[0]= -1;
            info.character = word[i];
            info.isPresent = 1;
            info.minNumberOccurrences = 1;
            info.exactNumberOccurrences = -1;
            updateFilter(info);
            updateCharacterArray(info);

        }
        else {
            char *result = strchr(reference, word[i]);
            if (result == NULL) {
                ris[i]= '/';
                info.character= word[i];
                info.isPresent = 0;
                updateFilter(info);
                updateCharacterArray(info);

            } else {
                for ( j = 0; j < dim; j++) {
                    if (word[i] == reference[j]) {
                        nWordInReference++;

                    }
                    if (word[i] == word[j] && i!=j){
                        if(word[j]==reference[j]) {
                            nCorrectInWord++;
                        }
                    }


                }
                for(g=0;g<i;g++){
                    if (word[i]==word[g] && word[g]!=reference[g]){
                        nWrongInWord++;
                    }
                }
                if (nWrongInWord >= nWordInReference-nCorrectInWord){
                    ris[i] ='/';
                    info.character = word[i];
                    info.isPresent = 1;
                    info.minNumberOccurrences = nWordInReference;
                    info.exactNumberOccurrences = nWordInReference;
                    info.notPosition[0] = i;
                    info.exactPositions[0]=-1;
                    updateFilter(info);
                    updateCharacterArray(info);

                }else{
                    ris[i] ='|';

                }
            }

        }
    }
    for(int j=0;j<dim;j++ ){
        if (ris[j] == '|'){
            info.character = word[j];
            info.isPresent = 1;
            for(int h=0;h<dim;h++){
                if (word[j] == word[h] && j !=h){
                    if(ris[h] == '+' || ris[h]=='|') {
                        atLeast ++;
                    }
                }
            }
            info.minNumberOccurrences = atLeast;
            info.exactNumberOccurrences = -1;
            info.exactPositions[0] = -1;
            info.notPosition[0] = j;
            updateFilter(info);
            updateCharacterArray(info);
            atLeast = 1;
        }
    }
    printf("%s\n",ris);
    free(ris);
}void FilterWordQuick(node_t* node){
    int minNumb = 0,exactNumb = 0;
    //int length = strlen(node->key);
    for (int i = 0; i < FILTER; i++) {
        if (filterArray[i].character == '\0') break;
        do {
            if (filterArray[i].isPresent == 0) { // 1- the character is not in the reference word
                for (int j=0;j<FILTER;j++){
                    if(node->key[j] == filterArray[i].character){
                        node->flag = false;
                        break;
                    }
                }
                break;
            }else{
                if (strchr(node->key, filterArray[i].character) == NULL) {
                    node->flag = false; // character is in the reference word
                    break;
                }
            }
            for (int n = 0; n < ARR_LENGHT; n++) { //exact position
                if (filterArray[i].exactPositions[n] != -1) {
                    if (node->key[filterArray[i].exactPositions[n]] != filterArray[i].character) {
                        node->flag = false;
                        break;
                    }
                }

            }
            if (node->flag == false) break;
            for (int k = 0; k < ARR_LENGHT; k++) { // notPosition
                if (filterArray[i].notPosition[k] != -1) {
                    if (node->key[filterArray[i].notPosition[k]] == filterArray[i].character) {
                        node->flag = false;
                        break;
                    }
                }
            }
            if (node->flag == false) break;


            if (filterArray[i].minNumberOccurrences > 1) { //min number occurrences
                for (int s = 0; s < FILTER; s++) {
                    if (node->key[s] == filterArray[i].character) {
                        minNumb++;
                    }
                }
                if (minNumb < filterArray[i].minNumberOccurrences) {
                    node->flag = false;
                    break;
                }
                minNumb = 0;
            }

            if (filterArray[i].exactNumberOccurrences != -1) { // exact number occurrencies
                for (int u = 0; u < FILTER; u++) {
                    if (node->key[u] == filterArray[i].character) {
                        exactNumb++;
                    }
                }
                if (exactNumb != filterArray[i].exactNumberOccurrences) {
                    node->flag = false;
                    break;
                }
                exactNumb = 0;
            }

            break;


        } while (1);
        if (node->flag == false) break;
    }



}
void FilterWord(node_t* node) {
    int minNumb = 0, excNumb = 0;
    int length = strlen(node->key);
    for (int i = 0; i < MAX_CHARACTERS; i++) {
        if (characterArray[i].character == '\0') break;
        do {
            if (characterArray[i].isPresent == 0) { // 1- the character is not in the reference word
                if (strchr(node->key, characterArray[i].character) != NULL) {
                    node->flag = false;
                    break;
                }
                break;
            }

            if (characterArray[i].isPresent == 1) {
                if (strchr(node->key, characterArray[i].character) == NULL) {
                    node->flag = false; // character is in the reference word
                    break;
                }
            }
            for (int n = 0; n < ARR_LENGHT; n++) { //exact position
                if (characterArray[i].exactPositions[n] != -1) {
                    if (node->key[characterArray[i].exactPositions[n]] != characterArray[i].character) {
                        node->flag = false;
                        break;
                    }
                }

            }
            if (node->flag == false) break;
            for (int k = 0; k < ARR_LENGHT; k++) { // notPosition
                if (characterArray[i].notPosition[k] != -1) {
                    if (node->key[characterArray[i].notPosition[k]] == characterArray[i].character) {
                        node->flag = false;
                        break;
                    }
                }
            }
            if (node->flag == false) break;
            if (characterArray[i].minNumberOccurrences > 1) { //min number occurrences
                for (int j = 0; j < length; j++) {
                    if (node->key[j] == characterArray[i].character) minNumb++;
                }
                if (minNumb < characterArray[i].minNumberOccurrences) {
                    node->flag = false;
                    break;
                }
                minNumb = 0;
            }

            if (characterArray[i].exactNumberOccurrences != -1) { // exact number occurrencies
                for (int h = 0; h < length; h++) {
                    if (node->key[h] == characterArray[i].character) excNumb++;
                }
                if (excNumb != characterArray[i].exactNumberOccurrences) {
                    node->flag = false;
                    break;
                }
                excNumb = 0;
            }

            break;


        } while (1);
        if (node->flag == false) break;
    }
}



void FilterWordQuickList(Node* node){
    int minNumb = 0,excNumb=0;
    //int length = strlen(node->key);
    for (int i = 0; i < FILTER; i++) {
        if (filterArray[i].character == '\0') break;
        do {
            if (filterArray[i].isPresent == 0) { // 1- the character is not in the reference word
                for (int j=0;j<FILTER;j++){
                    if(node->word[j] == filterArray[i].character){
                        node->isPresent = false;
                        break;
                    }
                }
                break;
            }else{
                if (strchr(node->word, filterArray[i].character) == NULL) {
                    node->isPresent = false; // character is in the reference word
                    break;
                }
            }
            if (node->isPresent == false) break;
            for (int n = 0; n < ARR_LENGHT; n++) { //exact position
                if (filterArray[i].exactPositions[n] != -1) {
                    if (node->word[filterArray[i].exactPositions[n]] != filterArray[i].character) {
                        node->isPresent = false;
                        break;
                    }
                }

            }
            if (node->isPresent == false) break;
            for (int k = 0; k < ARR_LENGHT; k++) { // notPosition
                if (filterArray[i].notPosition[k] != -1) {
                    if (node->word[filterArray[i].notPosition[k]] == filterArray[i].character) {
                        node->isPresent = false;
                        break;
                    }
                }
            }
            if (node->isPresent == false) break;
            if (filterArray[i].minNumberOccurrences > 1) { //min number occurrences
                for (int j = 0; j < FILTER; j++) {
                    if (node->word[j] == filterArray[i].character) minNumb++;
                }
                if (minNumb < filterArray[i].minNumberOccurrences) {
                    node->isPresent = false;
                    break;
                }
                minNumb = 0;
            }

            if (filterArray[i].exactNumberOccurrences != -1) { // exact number occurrencies
                for (int h = 0; h < FILTER; h++) {
                    if (node->word[h] == filterArray[i].character) excNumb++;
                }
                if (excNumb != filterArray[i].exactNumberOccurrences) {
                    node->isPresent = false;
                    break;
                }
                excNumb = 0;
            }

            break;


        } while (1);
        if (node->isPresent == false) break;
    }



}
void FilterWord2(Node* node){
    int minNumb = 0,excNumb=0;
    int length = strlen(node->word);
    for (int i = 0; i < MAX_CHARACTERS; i++) {
        if(characterArray[i].character == '\0') break;
        do{ if(characterArray[i].character == '\0') break;
            if(characterArray[i].isPresent == 0){ // 1- the character is not in the reference word
                if(strchr(node->word,characterArray[i].character) != NULL) {
                    node->isPresent = false;
                    break;
                }
                break;
            }

            if (characterArray[i].isPresent == 1){
                if (strchr(node->word,characterArray[i].character) == NULL){
                    node->isPresent = false; // character is in the reference word
                    break;
                }
            }
            if(characterArray[i].minNumberOccurrences > 1){ //min number occurrences
                for(int j=0;j<length;j++){
                    if (node->word[j] == characterArray[i].character) minNumb++;
                }
                if (minNumb < characterArray[i].minNumberOccurrences) {
                    node->isPresent = false;
                    break;
                }
                minNumb = 0;
            }
            if(characterArray[i].exactNumberOccurrences != -1){ // exact number occurrencies
                for(int h=0;h<length;h++){
                    if (node->word[h] == characterArray[i].character) excNumb++;
                }
                if (excNumb != characterArray[i].exactNumberOccurrences) {
                    node->isPresent = false;
                    break;
                }
                excNumb = 0;
            }
            for (int k = 0; k < ARR_LENGHT; k++) { // notPosition
                if (characterArray[i].notPosition[k] != -1){
                    if (node->word[characterArray[i].notPosition[k]] == characterArray[i].character){
                        node->isPresent = false;
                        break;
                    }
                }
            }
            if(node->isPresent == false) break;
            for (int n = 0; n < ARR_LENGHT; n++) { //exact position
                if (characterArray[i].exactPositions[n] != -1){
                    if (node->word[characterArray[i].exactPositions[n]] != characterArray[i].character){
                        node->isPresent = false;
                        break;
                    }
                }

            }
            break;
        } while (1);
        if (node->isPresent == false) break;
    }

}
void FilterTree(node_t *tree,Node**head,int*count,int dim){
    if (tree != NULL){
        FilterTree(tree->s_lef,head,count,dim);
        if (tree->flag == true) {
            FilterWordQuick(tree);
            if (tree->flag == true) {
                *head = insertAtTail(*head, tree->key, dim);
                (*count)++;
            }
        }
        FilterTree(tree->s_right,head,count,dim);
    }
}
void FilterTree1(node_t *tree,int*count){
    if (tree != NULL){
        FilterTree1(tree->s_lef,count);
        if (tree->flag == true) {
            FilterWordQuick(tree);
            if (tree->flag == true) {
                (*count)++;
            }

        }
        FilterTree1(tree->s_right,count);
    }
}
Node *FilterList(Node *head,int*count) {
    Node *current = head;
    Node *previous = NULL;

    while (current != NULL) {
        FilterWordQuickList(current);
        if (current->isPresent == false) {
            if (previous == NULL) {
                head = current->next;
                free(current);
                current = head;
            } else {
                previous->next = current->next;
                free(current);
                current = previous->next;
            }
        } else {
            (*count)++;
            previous = current;
            current = current->next;
        }
    }

    return head;
}

int main(int argc, char *argv[]) {
    initializeCharacterArray();
    initializeFilter();
    node_t *tree,*node;
    Node** head = (Node**)malloc(sizeof(Node*));
    *head = NULL;
    char *word;
    int dimW, dim = 0,numberConfrontation=0;
    int first;
    int threshold = 100000;
    int count = 0;
    int time = 0;
    if(scanf("%d\n",&dim)==0) return 0;
    dimW = max(dim + 1, 19);
    word = malloc(dimW*sizeof(char)); // to do: think a way to create the space to store all the words
    tree = createTree(dim);
    while (fgets(word, dimW, stdin) != NULL) {
        if (word[0] == '+') {
            if (word[1] == 'n') {
                char *wordReference = malloc((dim + 1) * sizeof(char));
                if (scanf("%s\n", wordReference) == 0) return 0;
                    wordReference[dim] = '\0';
                if (scanf("%d\n", &numberConfrontation) == 0) return 0;
                first = numberConfrontation;
                if (numberConfrontation != 0) {
                    do {
                        if (fgets(word, dimW, stdin) == 0)return 0;
                        if (word[0] == '+') {
                            if (word[1] == 's') {
                                if (threshold < THRESHOLD && time != 0) {
                                    printList(*head);
                                } else printkey(tree->father);

                            } else {
                                do {
                                    if (fgets(word, dimW, stdin)==0 ) return 0;
                                        if (word[0] == '+') {
                                            break;
                                        }
                                    node = createNode(word, dim);
                                    if (first != numberConfrontation) {
                                        FilterWord(node);
                                    }
                                    if (threshold < THRESHOLD && time != 0) {
                                        if (node->flag == true) {
                                            *head = insertInOrder(*head, node->key, dim);
                                        }
                                    }
                                    insertNode(tree, node);

                                } while (1);
                            }
                        } else {
                            word[dim] = '\0';
                            if (WordSearch(tree->father, word) == false) {
                                printf("not_exists\n");

                            } else {
                                if (strcmp(wordReference, word) == 0) {
                                    printf("ok\n");
                                    numberConfrontation = 0;
                                } else {
                                    WordConfrontation(wordReference, word);
                                    if (threshold > THRESHOLD) {
                                        FilterTree1(tree->father, &count);
                                    } else {
                                        if (time == 0) {
                                            FilterTree(tree->father, head, &count, dim);
                                            time++;
                                        } else *head = FilterList(*head, &count);

                                    }
                                    printf("%d\n", count);
                                    threshold = count;
                                    count = 0;
                                    initializeFilter();
                                    numberConfrontation--;
                                    if (numberConfrontation == 0) printf("ko\n");
                                }
                            }

                        }


                    } while (numberConfrontation != 0);
                    reinitializeTree(tree->father);
                    initializeCharacterArray();
                    time = 0;
                    threshold = 100000;
                    free(wordReference);
                    *head = NULL;
                }
            }


            else   if (word[1] == 's') {
                    printkey(tree->father);
                } else {
                    do {
                        if (fgets(word, dimW, stdin) == 0)return 0;
                        if (word[0] == '+') break;
                        node = createNode(word, dim);
                        insertNode(tree, node);

                    } while (1);
                }
            }
        else {
            node = createNode(word,dim);
            insertNode(tree, node);
        }
    }

    return 0;



}

