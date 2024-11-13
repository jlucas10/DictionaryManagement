#include <sys/stat.h> // contains struct for filesize info in getFileSize()
#include <iostream>
#include <fstream>
#include <vector>
#include <string>

#define DEFAULT_DICT "dictionary1.txt"
#define DEFAULT_OUTPUT_FILE "dict-updated.txt"

const int max_dict_option = 10;
const int min_dict_option = 1;

// Assumptions and constants for file operations
const double AVERAGE_WORD_LENGTH = 7;   // assumption about size of words in dictionaries
const double CUSHION = 1.25;            // how much beyond projected size should we reserve
const unsigned int DEFAULT_SIZE = 1000; // default size if we can't estimate

using namespace std;

enum Actions
{
    QUIT = 0,
    COUNT_WORDS = 1,
    PRINT_TO_SCREEN = 2,
    LINEAR_SEARCH = 3,
    DELETE_WORD = 4,
    BINARY_SEARCH = 8,
    INSERT_IN_ORDER = 9,
    SWAP = 5,
    BUBBLE_SORT = 6,
    SELECTION_SORT = 7,
    MERGE_DICTS = 10,
    LOAD_NEW_DICT = 11,
    WRITE_DICT_TO_FILE = 12
};

const int max_menu_option = WRITE_DICT_TO_FILE;

// Function prototypes
int getMenuChoice();
bool loadDictionary(const string &filename, vector<string> &words, bool verbose = false);
void countWords(const vector<string> &words, int &current_dict);
void printWords(const vector<string> &words);
int openNewDictionary(vector<string> &words, int new_dict, int &current_dict, bool verbose = false);
int loadNewDictionary(vector<string> &words, int &current_dict, bool verbose = false);
void writeDictionaryToFile(const vector<string> &words);
/* use C-library to get filesize in characters; overload to take either C or C++ string */
double getFileSize(const string &filename);
double getFileSize(const char *filename);
void findWordLinear(const vector<string> &words);
vector<string>::const_iterator findWord(string userWord, const vector<string> &vDict);
void deleteWords(vector<string> &words);
vector<string>::const_iterator binarySearch(string userWord, const vector<string> &vDict);
vector<string>::const_iterator binarySearch(string userWord, const vector<string> &vDict,
                                            vector<string>::const_iterator startIT,
                                            vector<string>::const_iterator endIT);

int main()
{
    vector<string> words;
    int current_dict = 1;
    bool verbose = false;
    loadDictionary(DEFAULT_DICT, words, verbose);

    int choice;
    do
    {
        choice = getMenuChoice();

        switch (choice)
        {
        case COUNT_WORDS:
            countWords(words, current_dict);
            break;
        case PRINT_TO_SCREEN:
            printWords(words);
            break;
        case LINEAR_SEARCH:
            findWordLinear(words);
            break;
        case DELETE_WORD:
            deleteWords(words);
            break;
        case BINARY_SEARCH:
            findWordLinear(words);
            break;
        case INSERT_IN_ORDER:
            cout << "Coming soon!\n";
            break;
        case SWAP:
            cout << "Coming soon!\n";
            break;
        case BUBBLE_SORT:
            cout << "Coming soon!\n";
            break;
        case SELECTION_SORT:
            cout << "Coming soon!\n";
            break;
        case MERGE_DICTS:
            cout << "Coming soon!\n";
            break;
        case LOAD_NEW_DICT:
            if (loadNewDictionary(words, current_dict, false) != 0)
            {
                return 0;
            }
            break;
        case WRITE_DICT_TO_FILE:
            writeDictionaryToFile(words);
            break;
        case QUIT:
            cout << "Thank you! Bye!\n";
            break;
        default:
            cout << "Invalid choice! Please try again.\n";
        }
    } while (choice != QUIT);

    return 0;
}

int getMenuChoice()
{
    int choice;

    while (true) // Loop until a valid choice is made
    {
        // Display the menu
        cout << "--------------------------------------------\n";
        cout << "Options menu:\n";
        cout << " (1) Count - get number of words in dictionary\n";
        cout << " (2) Print words to screen\n";
        cout << " (3) Find a word (Linear Search)\n";
        cout << " (4) Find word, delete if found\n";
        cout << " (5) Swap two words\n";
        cout << " (6) Sort words (Bubble Sort)\n";
        cout << " (7) Sort words (Selection Sort)\n";
        cout << " (8) Find a word - Binary Search (assumes words are sorted alphabetically)\n";
        cout << " (9) Find word, insert if not found (assumes words are sorted alphabetically)\n";
        cout << " (10) Merge two dictionaries (will sort first)\n";
        cout << " (11) Load a dictionary (closes current dictionary)\n";
        cout << " (12) Write current dictionary to file\n";
        cout << "Enter a number from 1 to 12, or 0 to exit: \n";

        cin >> choice;

        // Check input
        if (cin.fail() || choice < QUIT || choice > max_menu_option)
        {
            cout << "Error! Input must be a number between 1 and 12, or 0 to exit.\n";
            cin.clear();
        }
        else
        {
            break;
        }
    }

    return choice;
}

bool loadDictionary(const string &filename, vector<string> &words, bool verbose)
{
    ifstream file(filename);
    if (!file)
    {
        cout << "Error: Could not open file " << filename << endl;
        return false;
    }

    words.resize(30);

    if (verbose)
    {
        cout << "##Vector size to start:          " << words.size() << "  capacity: " << words.capacity() << "##" << endl;
    }

    words.clear();

    if (verbose)
    {
        cout << "##Vector size after .clear():     " << words.size() << "    capacity: " << words.capacity() << "##" << endl;
    }

    double fileSize = getFileSize(filename);
    double estimatedWords = fileSize / AVERAGE_WORD_LENGTH;
    double reserveSize = static_cast<double>(estimatedWords * CUSHION);

    words.reserve(reserveSize);

    if (verbose)
    {
        cout << "##Vector size after reserve:     " << words.size() << "    capacity: " << words.capacity()
             << "    filesize: " << fileSize << "   reserve:   " << reserveSize << "##" << endl;
    }

    string word;
    while (file >> word)
    {
        words.push_back(word);
    }
    file.close();

    if (verbose)
    {
        cout << "##Vector size when filled:       " << words.size() << "     capacity: " << words.capacity() << "##" << endl;
    }

    return true;
}

void countWords(const vector<string> &words, int &current_dict)
{
    cout << "There are " << words.size() << " words in dictionary #" << current_dict << "." << endl;
}

void printWords(const vector<string> &words)
{
    for (unsigned int i = 0; i < words.size(); ++i)
    {
        cout << words[i] << endl;
    }
}

int openNewDictionary(vector<string> &words, int new_dict, int &current_dict, bool verbose)
{
    string filename = "dictionary" + to_string(new_dict) + ".txt";
    ifstream file(filename);
    if (!file)
    {
        cout << "ERROR! Cannot read chosen dictionary \"" << filename << "\". Exiting." << endl;
        return 1;
    }

    words.clear();
    loadDictionary(filename, words, verbose);

    current_dict = new_dict;
    cout << "Dictionary " << current_dict << " is open." << endl;

    return 0;
}

int loadNewDictionary(vector<string> &words, int &current_dict, bool verbose)
{
    int new_dict;

    while (true)
    {
        cout << "Which Dictionary should be opened? Enter a number between \"" << min_dict_option << "\" and \"" << max_dict_option << "\": " << endl;
        cin >> new_dict;

        if (cin.fail() || new_dict < min_dict_option || new_dict > max_dict_option)
        {
            cout << "That number is not in the available range! Pick another.\n";
            cin.clear();
        }
        else if (new_dict == current_dict)
        {
            cout << "That dictionary is already open! Pick another." << endl;
        }
        else
        {
            break;
        }
    }
    return openNewDictionary(words, new_dict, current_dict, verbose);
}

void writeDictionaryToFile(const vector<string> &words)
{
    string filename;
    cout << "Enter name of file (.txt will be appended automatically), or enter 'd' to use default filename: " << endl;
    cin >> filename;

    if (filename == "d")
    {
        filename = DEFAULT_OUTPUT_FILE;
    }
    else
    {
        filename += ".txt";
    }

    ifstream check_file(filename);
    if (check_file.good())
    {
        cout << "ERROR! The file " << filename << " already exists." << endl;
        return;
    }

    ofstream file(filename);
    if (!file)
    {
        cout << "Error writing to file: " << filename << endl;
        return;
    }

    for (const string &word : words)
    {
        file << word << endl;
    }
    file.close();
    cout << "Writing to file ...\n                   ...Done!" << endl;
}

/* Implementation of getFileSize for C++ string */
double getFileSize(const string &filename)
{
    return getFileSize(filename.c_str());
}

/* Implementation of getFileSize for C string */
double getFileSize(const char *filename)
{
    struct stat fileInfo;
    int fail = stat(filename, &fileInfo);
    if (fail == -1)
    { // Error if file does not exist
        return fail;
    }
    return fileInfo.st_size; // Returns file size in characters
}

void findWordLinear(const vector<string> &words)
{
    string userWord;
    cout << "Enter a word to find: Your word was '";
    cin >> userWord;

    vector<string>::const_iterator it = findWord(userWord, words);

    if (it != words.end())
    {
        cout << userWord << "'. ";
        if (it + 1 != words.end())
        {
            cout << "The next word would be '" << *(it + 1) << "'." << endl;
        }
        else
        {
            cout << "It is the last word." << endl;
        }
    }
    else
    {
        cout << userWord << "'. We did not find your word." << endl;
    }
}

vector<string>::const_iterator findWord(string userWord, const vector<string> &vDict)
{
    for (vector<string>::const_iterator it = vDict.begin(); it != vDict.end(); ++it)
    {
        if (*it == userWord)
        {
            return it;
        }
    }
    return vDict.end();
}

void deleteWords(vector<string> &words)
{
    string userWord;
    cout << "Enter a word to delete from the chosen Dictionary: ";
    cin >> userWord;

    vector<string>::const_iterator it = findWord(userWord, words);

    if (it != words.end())
    {
        cout << "Your word was '" << userWord << "'. The word '" << userWord << "' has been deleted." << endl;
        words.erase(it);
    }
    else
    {
        cout << "Your word was '" << userWord << "'. We did not find your word." << endl;
    }
}

vector<string>::const_iterator binarySearch(string userWord, const vector<string> &vDict)
{
    return binarySearch(userWord, vDict, vDict.begin(), vDict.end());
}
vector<string>::const_iterator binarySearch(string userWord, const vector<string> &vDict, vector<string>::const_iterator startIT, vector<string>::const_iterator endIT)
{
    if (startIT >= endIT)
    {
        return endIT; // not found
    }

    vector<string>::const_iterator midIT = startIT + (endIT - startIT) / 2;

    if (*midIT == userWord)
    {
        return midIT; // found
    }
    else if (*midIT < userWord)
    {
        return binarySearch(userWord, vDict, midIT + 1, endIT); // search right half
    }
    else
    {
        return binarySearch(userWord, vDict, startIT, midIT); // search left half
    }
}