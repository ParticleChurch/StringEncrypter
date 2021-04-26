#include <iostream>
#include <fstream>
#include <random>
#include <string>

inline uint32_t random()
{
	static std::mt19937_64 rng(std::random_device{}());
	return rng() % (uint32_t)-1;
}

#define MINSTRINGSIZE (sizeof("!XOR!\0") - 1)
void encryptStrings(char* f, size_t size)
{
	for (size_t filePosition = 0, nBytesRemaining = size; nBytesRemaining >= MINSTRINGSIZE; filePosition++, nBytesRemaining--)
	{
		char* tag = f + filePosition;
		if (strncmp(tag, "!XOR", 4))
			continue;
		
		size_t stringSize = 0;
		char* string = nullptr;
		if (tag[4] == '!')
		{
			if (nBytesRemaining <= 5) return;
			stringSize = strnlen_s(tag + 5, nBytesRemaining - 5) + 1; // +=1 for null terminator
			if (stringSize > nBytesRemaining) return;
			string = tag + 5;
		}
		else
		{
			for (size_t i = 4; i < nBytesRemaining; i++)
			{
				if ('0' <= tag[i] && tag[i] <= '9')
				{
					stringSize = stringSize * 10 + (tag[i] - '0');
				}
				else if (tag[i] == '!')
				{
					string = tag + i + 1;
					break;
				}
				else
				{
					std::cout << "Found near match at file position " << filePosition << std::endl;
					break;
				}
			}
			if (!string)
				continue;
		}


		uint32_t key = random();
		std::cout << key << "/" << stringSize << " : " << std::string(string, std::min(stringSize, (size_t)16))<< std::endl;

		tag[0] = '^';
		*(uint32_t*)(tag + 1) = key;

		size_t offset = 0;
		for (; stringSize >= 4 && offset <= stringSize - 4; offset += 4)
			*(uint32_t*)(string + offset) ^= key;
		for (; offset < stringSize; offset++)
			string[offset] ^= key & 0xff;
	}
}

int main()
{
	srand((unsigned int)(time(0)));
	std::fstream ifile;
	std::fstream ofile;
	ifile.open("E:\\GitHub\\internalV1\\Release\\CSGOCollabV1.dll", std::fstream::in | std::fstream::binary);
	ofile.open("E:\\GitHub\\internalV1\\Release\\encrypted.dll", std::fstream::out | std::fstream::binary);

	if (!ifile.is_open() || !ofile.is_open())
	{
		std::cout << "Failed to open file(s)." << std::endl;
		return 1;
	}

	ifile.seekg(0, std::fstream::end);
	size_t size = (size_t)(ifile.tellg());
	ifile.seekg(0);

	char* f = new char[size];
	if (!ifile.read(f, size))
	{
		std::cout << "Failed to read file." << std::endl;
		return 1;
	}

	encryptStrings(f, size);

	if (!ofile.write(f, size))
	{
		std::cout << "Failed to write output." << std::endl;
		return 1;
	}

	ifile.close();
	ofile.close();

	return 0;
};