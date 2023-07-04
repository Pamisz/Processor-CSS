#include <iostream>
#include <cstdlib>

using namespace std;

#define T 8

class myString {
private:
	char* buffer;
	size_t size;

public:
	myString()
		:buffer(nullptr), size(0) {}
	myString(const char* s)
		:size(strlen(s))
	{
		buffer = new char[size + 1];
		strcpy_s(buffer, size + 1, s);
	}

	size_t getSize() {
		return size;
	}

	void sizeDown() {
		size--;
	}

	const char* string() const {
		return buffer;
	}

	myString& operator+=(char c) {
		char* newBuffer = new char[size + 2];

		if (buffer != nullptr) {
			strcpy_s(newBuffer, size + 2, buffer);
			delete[] buffer;
		}

		newBuffer[size] = c;
		newBuffer[size + 1] = '\0';

		buffer = newBuffer;
		size++;

		return *this;
	}

	bool operator==(const myString& other) const {
		if (size != other.size) {
			return false;
		}
		return strcmp(buffer, other.buffer) == 0;
	}
	char& operator[](const int index) {
		return buffer[index];
	}
};

class selector {
public:
	selector* next = nullptr;
	selector* prev = nullptr;
	myString name;
};

class selectors_list {
public:
	selector* front = nullptr;
	selector* back = nullptr;

	void create_node(selector* node) {
		if (!front) {
			front = node;
		}
		else {
			node->next = back;
			back->prev = node;
		}
		back = node;
	}
};

class attribute {
public:
	attribute* next = nullptr;
	attribute* prev = nullptr;
	myString property;
	myString value;
};

class attributes_list {
public:
	attribute* front = nullptr;
	attribute* back = nullptr;

	void create_node(attribute* attr) {
		if (!front) {
			front = attr;
		}
		else {
			attr->next = back;
			back->prev = attr;
		}
		back = attr;
	}
};

class section {
public:
	selectors_list selectors;
	attributes_list attributes;
};

class block {
public:
	section blk[T];
	block* prev = nullptr;
	block* next = nullptr;
};

class block_list {
public:
	block* front = nullptr;
	block* back = nullptr;

	void create_node(block* blk) {
		if (!front) {
			front = blk;
		}
		else {
			blk->next = back;
			back->prev = blk;
		}
		back = blk;
	}
};

void loadSelectors(section* scn, char letter) {
	myString name("");
	if (letter == '{') {
		selector* empty = new selector;
		empty->name = name;
		scn->selectors.create_node(empty);
	}
	while (letter != '{') {
		bool duplicate = false;
		if (letter == ',') {
			letter = getchar();
		}
		while (isspace(letter)) {
			letter = getchar();
		}
		while (letter != ',' && letter != '{') {
			name += letter;
			letter = getchar();
		}
		while (isspace(name[name.getSize() - 1])) {
			name[name.getSize() - 1] = '\0';
			name.sizeDown();
		}
		selector* s = new selector;
		s->name = name;
		name = "";
		if (scn->selectors.front != nullptr) {
			selector* selectorBuff = scn->selectors.front;
			while (selectorBuff != nullptr) {
				if (selectorBuff->name == s->name) {
					duplicate = true;
					break;
				}
				selectorBuff = selectorBuff->prev;
			}
		}
		if (duplicate == false) {
			scn->selectors.create_node(s);
		}
	}
}

void loadAttributes(section* scn) {
	myString buffer("");
	char letter = 'l';
	while (letter != '}') {
		bool overwriting = false;
		letter = getchar();
		attribute* attr = new attribute;
		while (letter != ':' && letter != '}') {
			if (!isspace(letter)) {
				buffer += letter;
			}
			letter = getchar();
		}
		if (letter == '}') {
			break;
		}
		if (buffer.getSize() > 0) {
			attr->property = buffer;
			buffer = "";
		}
		letter = getchar();
		while (letter != ';' && letter != '}') {
			buffer += letter;
			letter = getchar();
		}
		if (scn->attributes.front != nullptr) {
			attribute* ptr = scn->attributes.front;
			while (ptr != nullptr && !overwriting) {
				if (ptr->property == attr->property) {
					if (buffer.getSize() > 0) {
						ptr->value = buffer;
						buffer = "";
					}
					overwriting = true;
				}
				else {
					ptr = ptr->prev;
				}
			}
		}
		if (!overwriting) {
			if (buffer.getSize() > 0) {
				attr->value = buffer;
				buffer = "";
			}
			scn->attributes.create_node(attr);
		}
	}
}

void loadSection(section* scn, char letter) {
	loadSelectors(scn, letter);
	loadAttributes(scn);
}

void loadContent(block_list* bl);

bool isDigit(char c) {
	return c >= '0' && c <= '9';
}

void allSections(block_list* bl) {
	int meter = 0;
	if (bl->front != nullptr) {
		block* buffer = bl->front;
		while (buffer != nullptr) {
			for (int i = 0; i < T; i++) {
				if (buffer->blk[i].attributes.front != nullptr) {
					meter++;
				}
			}
			buffer = buffer->prev;
		}
	}
	std::cout << "? == " << meter << endl;
}

void selectorsForSectionNr(block_list* bl, int index) {
	if (bl->front != nullptr) {
		int meter = 0, selectors = 0;
		block* buffer = bl->front;
		section* sectionBuff = nullptr;
		while (buffer != nullptr) {
			for (int i = 0; i < T; i++) {
				if (buffer->blk[i].attributes.front != nullptr) {
					meter++;
				}
				if (meter == index) {
					sectionBuff = &buffer->blk[i];
					break;
				}
			}
			if (meter == index) {
				if (sectionBuff->selectors.front != nullptr) {
					selector* selectorBuff = sectionBuff->selectors.front;
					if (selectorBuff->name == "") {
						std::cout << index << ",S,? == 0" << endl;
						break;
					}
					selectors = 1;
					while (selectorBuff != nullptr) {
						selectorBuff = selectorBuff->prev;
						if (selectorBuff != nullptr) {
							selectors++;
						}
					}
					std::cout << index << ",S,? == " << selectors << endl;
					break;
				}
			}
			else {
				buffer = buffer->prev;
			}
		}
	}
}

void selectorNrFromBlockNr(block_list* bl, int sectionIndex, int selectorIndex) {
	if (bl->front != nullptr) {
		int meter = 0, selectors = 0;
		block* buffer = bl->front;
		section* sectionBuff = nullptr;
		while (buffer != nullptr) {
			for (int i = 0; i < T; i++) {
				if (buffer->blk[i].attributes.front != nullptr) {
					meter++;
				}
				if (meter == sectionIndex) {
					sectionBuff = &buffer->blk[i];
					break;
				}
			}
			if (meter == sectionIndex) {
				if (sectionBuff->selectors.front != nullptr) {
					selector* selectorBuff = sectionBuff->selectors.front;
					selectors = 1;
					if (selectorIndex == 1 && !(selectorBuff->name == "")) {
						std::cout << sectionIndex << ",S,1 == " << selectorBuff->name.string() << endl;
						break;
					}
					else {
						while (selectorBuff != nullptr) {
							selectorBuff = selectorBuff->prev;
							if (selectorBuff != nullptr) {
								selectors++;
								if (selectors == selectorIndex && !(selectorBuff->name == "")) {
									std::cout << sectionIndex << ",S," << selectorIndex << " == " << selectorBuff->name.string() << endl;
									break;
								}
							}
						}
					}
				}
			}
			else {
				buffer = buffer->prev;
			}
		}
	}
}

void selectorNamed(block_list* bl, myString name) {
	int meter = 0;
	if (bl->front != nullptr) {
		selector* buffer;
		block* buffBlk = bl->front;
		while (buffBlk != nullptr) {
			for (int i = 0; i < T; i++) {
				if (buffBlk->blk[i].selectors.front != nullptr) {
					buffer = buffBlk->blk[i].selectors.front;
					if (buffer->name == name) {
						meter++;
					}
					while (buffer->prev != nullptr) {
						buffer = buffer->prev;
						if (buffer->name == name) {
							meter++;
						}
					}
				}
			}
			buffBlk = buffBlk->prev;
		}
	}
	std::cout << name.string() << ",S,? == " << meter << endl;
}

void attributesforSectionNr(block_list* bl, int index) {
	if (bl->front != nullptr) {
		block* blockBuff = bl->front;
		section* sectionBuff = nullptr;
		int meter = 0, attributes = 0;
		while (blockBuff != nullptr) {
			for (int i = 0; i < T; i++) {
				if (blockBuff->blk[i].attributes.front != nullptr) {
					meter++;
				}
				if (meter == index) {
					sectionBuff = &blockBuff->blk[i];
					break;
				}
			}
			if (meter == index) {
				if (sectionBuff->attributes.front != nullptr) {
					attribute* attributeBuff = sectionBuff->attributes.front;
					attributes = 1;
					while (attributeBuff != nullptr) {
						attributeBuff = attributeBuff->prev;
						if (attributeBuff != nullptr) {
							attributes++;
						}
					}
					std::cout << index << ",A,? == " << attributes << endl;
					break;
				}
			}
			else {
				blockBuff = blockBuff->prev;
			}
		}
	}
}

void attributeFromSectionNr(block_list* bl, int index, myString name) {
	if (bl->front != nullptr) {
		int meter = 0;
		myString tmpValue("");
		block* buffer = bl->front;
		section* sectionBuff = nullptr;
		while (buffer != nullptr) {
			for (int i = 0; i < T; i++) {
				if (buffer->blk[i].attributes.front != nullptr) {
					meter++;
				}
				if (meter == index) {
					sectionBuff = &buffer->blk[i];
					break;
				}
			}
			if (meter == index) {
				if (sectionBuff->attributes.front != nullptr) {
					attribute* attributeBuff = sectionBuff->attributes.front;
					if (attributeBuff->property == name) {
						tmpValue = attributeBuff->value;
					}
					while (attributeBuff != nullptr) {
						attributeBuff = attributeBuff->prev;
						if (attributeBuff != nullptr && attributeBuff->property == name) {
							tmpValue = attributeBuff->value;
						}
					}
					if (tmpValue.getSize() > 0) {
						std::cout << index << ",A," << name.string() << " == " << tmpValue.string() << endl;
						break;
					}
				}
			}
			else {
				buffer = buffer->prev;
			}
		}
	}
}

void attributesNamed(block_list* bl, myString name) {
	int meter = 0;
	if (bl->front != nullptr) {
		block* buffer = bl->front;
		attribute* attributeBuff = nullptr;
		while (buffer != nullptr) {
			for (int i = 0; i < T; i++) {
				if (buffer->blk[i].attributes.front != nullptr) {
					attributeBuff = buffer->blk[i].attributes.front;
					while (attributeBuff != nullptr) {
						if (attributeBuff->property == name) {
							meter++;
						}
						attributeBuff = attributeBuff->prev;
					}
				}
			}
			buffer = buffer->prev;
		}
	}
	std::cout << name.string() << ",A,? == " << meter << endl;
}

void attributeNamedFromSelectorNamed(block_list* bl, myString selectorName, myString attributeName) {
	if (bl->front != nullptr) {
		block* blockBuff = bl->front;
		selector* selectorBuff;
		attribute* attributeBuff;
		myString tmpValue("");
		while (blockBuff != nullptr) {
			for (int i = 0; i < T; i++) {
				if (blockBuff->blk[i].selectors.front != nullptr) {
					selectorBuff = blockBuff->blk[i].selectors.front;
					if (selectorBuff->name == selectorName) {
						if (blockBuff->blk[i].attributes.front != nullptr) {
							attributeBuff = blockBuff->blk[i].attributes.front;
							if (attributeBuff->property == attributeName) {
								tmpValue = attributeBuff->value;
							}
							while (attributeBuff != nullptr) {
								attributeBuff = attributeBuff->prev;
								if (attributeBuff != nullptr) {
									if (attributeBuff->property == attributeName) {
										tmpValue = attributeBuff->value;
									}
								}
							}
						}
					}
					while (selectorBuff != nullptr) {
						selectorBuff = selectorBuff->prev;
						if (selectorBuff != nullptr && selectorBuff->name == selectorName) {
							if (blockBuff->blk[i].attributes.front != nullptr) {
								attributeBuff = blockBuff->blk[i].attributes.front;
								if (attributeBuff->property == attributeName) {
									tmpValue = attributeBuff->value;
								}
								while (attributeBuff != nullptr) {
									attributeBuff = attributeBuff->prev;
									if (attributeBuff != nullptr) {
										if (attributeBuff->property == attributeName) {
											tmpValue = attributeBuff->value;
										}
									}
								}
							}
						}
					}
				}
			}
			blockBuff = blockBuff->prev;
		}
		if (tmpValue.getSize() > 0) {
			std::cout << selectorName.string() << ",E," << attributeName.string() << " == " << tmpValue.string() << endl;
		}
	}
}

void deleteSectionNr(block_list* bl, int index) {
	if (bl->front != nullptr) {
		int meter = 0;
		block* buffer = bl->front;
		section* sectionBuff = nullptr;
		selector* selectorBuff = nullptr;
		attribute* attributeBuff = nullptr;
		while (buffer != nullptr) {
			for (int i = 0; i < T; i++) {
				if (buffer->blk[i].attributes.front != nullptr) {
					meter++;
				}
				if (meter == index) {
					sectionBuff = &buffer->blk[i];
					break;
				}
			}
			if (meter == index && sectionBuff != nullptr) {
				if (sectionBuff->attributes.front != nullptr) {
					attributeBuff = sectionBuff->attributes.front;
					while (attributeBuff != nullptr) {
						attribute* tmpAttribute = attributeBuff;
						attributeBuff = attributeBuff->prev;
						delete tmpAttribute;
					}
					sectionBuff->attributes.front = nullptr;
					sectionBuff->attributes.back = nullptr;
					if (sectionBuff->selectors.front != nullptr) {
						selectorBuff = sectionBuff->selectors.front;
						while (selectorBuff != nullptr) {
							selector* tmpSelector = selectorBuff;
							selectorBuff = selectorBuff->prev;
							delete tmpSelector;
						}
						sectionBuff->selectors.front = nullptr;
						sectionBuff->selectors.back = nullptr;
					}
					std::cout << index << ",D,* == deleted" << endl;
					break;
				}
			}
			else {
				buffer = buffer->prev;
			}
		}
	}

}

void deleteAttributeNamedFromSectionNr(block_list* bl, int index, myString name) {
	if (bl->front != nullptr) {
		int meter = 0;
		block* buffer = bl->front;
		section* sectionBuff = nullptr;
		while (buffer != nullptr) {
			for (int i = 0; i < T; i++) {
				if (buffer->blk[i].attributes.front != nullptr) {
					meter++;
				}
				if (meter == index) {
					sectionBuff = &buffer->blk[i];
					break;
				}
			}
			if (meter == index) {
				if (sectionBuff->attributes.front != nullptr) {
					attribute* attributeBuff = sectionBuff->attributes.front;
					while (attributeBuff != nullptr) {
						if (attributeBuff->property == name) {
							if (sectionBuff->attributes.front == attributeBuff && sectionBuff->attributes.back == attributeBuff) {
								if (sectionBuff->selectors.front != nullptr) {
									selector* selectorBuff = sectionBuff->selectors.front;
									while (selectorBuff != nullptr) {
										selector* tmpSelector = selectorBuff;
										selectorBuff = selectorBuff->prev;
										delete tmpSelector;
									}
									delete selectorBuff;
									sectionBuff->selectors.front = nullptr;
									sectionBuff->selectors.back = nullptr;
								}
								sectionBuff->attributes.front = nullptr;
								sectionBuff->attributes.back = nullptr;
								delete attributeBuff;
							}
							else if (attributeBuff->next != nullptr && attributeBuff->prev != nullptr) {
								attributeBuff->prev->next = attributeBuff->next;
								attributeBuff->next->prev = attributeBuff->prev;
								delete attributeBuff;
							}
							else if (attributeBuff->next != nullptr) {
								sectionBuff->attributes.back = attributeBuff->next;
								attributeBuff->next->prev = nullptr;
								delete attributeBuff;
							}
							else if (attributeBuff->prev != nullptr) {
								sectionBuff->attributes.front = attributeBuff->prev;
								attributeBuff->prev->next = nullptr;
								delete attributeBuff;
							}
							std::cout << index << ",D," << name.string() << " == deleted" << endl;
							break;
						}
						attributeBuff = attributeBuff->prev;
					}

				}
			}
			buffer = buffer->prev;
		}
	}
}

void commands(block_list* bl) {
	bool running = true;
	while (running) {
		char letter = getchar();
		if (letter == EOF) {
			break;
		}
		myString buffer("");
		int commas[2] = { 1, 3 };
		while (letter != '\n') {
			if (letter == EOF) {
				running = false;
				break;
			}

			buffer += letter;
			letter = getchar();
		}
		for (int i = 0, k = 0; i < buffer.getSize(); i++) {
			if (buffer[i] == ',') {
				commas[k] = i;
				k++;
			}
		}
		if (buffer == "****") {
			loadContent(bl);
			break;
		}
		else if (buffer == "?") {
			allSections(bl);
		}
		else if (buffer[commas[0] + 1] == 'S') {
			if (isDigit(buffer[0])) {
				if (buffer[commas[1] + 1] == '?') {
					myString strIndex("");
					int meter = 0;
					while (buffer[meter] != ',') {
						strIndex += buffer[meter];
						meter++;
					}
					int index = atoi(strIndex.string());
					selectorsForSectionNr(bl, index);
				}
				else if (isDigit(buffer[commas[1] + 1])) {
					myString strSectionIndex("");
					myString strSelectorIndex("");
					int meter = 0;
					while (buffer[meter] != ',') {
						strSectionIndex += buffer[meter];
						meter++;
					}
					meter = commas[1] + 1;
					while (buffer[meter] != '\0') {
						strSelectorIndex += buffer[meter];
						meter++;
					}
					int sectionIndex = atoi(strSectionIndex.string());
					int selectorIndex = atoi(strSelectorIndex.string());
					selectorNrFromBlockNr(bl, sectionIndex, selectorIndex);
				}
			}
			else {
				if (buffer[commas[1] + 1] == '?') {
					myString name("");
					int meter = 0;
					while (buffer[meter] != ',') {
						name += buffer[meter];
						meter++;
					}
					selectorNamed(bl, name);
				}
			}
		}
		else if (buffer[commas[0] + 1] == 'A') {
			if (isDigit(buffer[0])) {
				if (buffer[commas[1] + 1] == '?') {
					myString strIndex("");
					int meter = 0;
					while (buffer[meter] != ',') {
						strIndex += buffer[meter];
						meter++;
					}
					int index = atoi(strIndex.string());
					attributesforSectionNr(bl, index);
				}
				else {
					myString strIndex("");
					int meter = 0;
					while (buffer[meter] != ',') {
						strIndex += buffer[meter];
						meter++;
					}
					int index = atoi(strIndex.string());
					meter = commas[1] + 1;
					myString name("");
					while (buffer[meter] != '\0') {
						name += buffer[meter];
						meter++;
					}
					attributeFromSectionNr(bl, index, name);
				}
			}
			else {
				if (buffer[commas[1] + 1] == '?') {
					myString name("");
					int meter = 0;
					while (buffer[meter] != ',') {
						name += buffer[meter];
						meter++;
					}
					attributesNamed(bl, name);
				}
			}
		}
		else if (buffer[commas[0] + 1] == 'E') {
			if (!isDigit(buffer[0]) && !isDigit(buffer[commas[1] + 1])) {
				myString selectorName("");
				int meter = 0;
				while (buffer[meter] != ',') {
					selectorName += buffer[meter];
					meter++;
				}
				meter = commas[1] + 1;
				myString attributeName("");
				while (buffer[meter] != '\0') {
					attributeName += buffer[meter];
					meter++;
				}
				attributeNamedFromSelectorNamed(bl, selectorName, attributeName);
			}
		}
		else if (buffer[commas[0] + 1] == 'D') {
			if (isDigit(buffer[0])) {
				if (buffer[commas[1] + 1] == '*') {
					myString strIndex("");
					int meter = 0;
					while (buffer[meter] != ',') {
						strIndex += buffer[meter];
						meter++;
					}
					int index = atoi(strIndex.string());
					deleteSectionNr(bl, index);
				}
				else {
					myString strIndex("");
					int meter = 0;
					while (buffer[meter] != ',') {
						strIndex += buffer[meter];
						meter++;
					}
					int index = atoi(strIndex.string());
					meter = commas[1] + 1;
					myString name("");
					while (buffer[meter] != '\0') {
						name += buffer[meter];
						meter++;
					}
					deleteAttributeNamedFromSectionNr(bl, index, name);
				}
			}
		}
	}
}

void loadContent(block_list* bl) {
	char letter = getchar();
	while (letter != EOF) {
		while (isspace(letter)) {
			letter = getchar();
		}
		if (letter == '?') {
			commands(bl);
			break;
		}
		else {
			if (bl->front == nullptr) {
				block* b = new block;
				loadSection(&b->blk[0], letter);
				bl->create_node(b);
			}
			else {
				block* buffer = bl->back;
				if (buffer->blk[T - 1].attributes.front == nullptr) {
					section* sectionBuff = &buffer->blk[T - 1];
					for (int i = (T - 2); i >= 0; i--) {
						if (buffer->blk[i].attributes.front == nullptr) {
							sectionBuff = &buffer->blk[i];
						}
						else {
							break;
						}
					}
					loadSection(sectionBuff, letter);
				}
				else {
					block* b2 = new block;
					loadSection(&b2->blk[0], letter);
					bl->create_node(b2);
				}
			}
			letter = getchar();
		}
	}
}

int main() {
	block_list* bl = new block_list;
	loadContent(bl);
	return 0;
}