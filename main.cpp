#include <iostream>
#include <fstream>
#include <random>
#include <string>

inline uint32_t random()
{
	static std::mt19937_64 rng(std::random_device{}());
	return rng() % (uint32_t)-1;
}

size_t encryptStrings(char* data, size_t size)
{
	size_t count = 0;
	for (size_t fpos = 0; fpos < size; fpos++)
	{
		if (strncmp(data + fpos, "!XOR!", 5) != 0)
			continue;

		// found a string!
		char* str = data + fpos;
		uint32_t key = random();
		const char* keys = (char*)(&key);

		std::cout << "Encrypting (" << key << "): " << str << std::endl;
		count++;

		*str = '^';
		*(uint32_t*)(str + 1) = key;

		size_t o = 0;
		while (true)
		{
			bool stop = str[5 + o] == '\0';
			str[5 + o] ^= keys[o % 4];

			if (stop)
				break;
			else
				o++;
		}
	}
	return count;
}

int main()
{
	srand((unsigned int)(time(0)));

	// read input file
	std::fstream ifile;
	ifile.open("E:\\GitHub\\internalV1\\Release\\CSGOCollabV1.dll", std::fstream::in | std::fstream::binary);
	if (!ifile.is_open())
	{
		std::cout << "Failed to open input file." << std::endl;
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
	ifile.close();

	// create output file
	std::fstream ofile;
	ofile.open("E:\\GitHub\\internalV1\\Release\\encrypted.dll", std::fstream::out | std::fstream::binary);
	if (!ofile.is_open())
	{
		std::cout << "Failed to open output file." << std::endl;
		return 1;
	}

	char* out = new char[size];
	memcpy(out, f, size);
	size_t count = encryptStrings(out, size);
	std::cout << "Encrypted a total of " << count << " strings." << std::endl;

	if (!ofile.write(out, size))
	{
		std::cout << "Failed to write output." << std::endl;
		return 1;
	}
	ofile.close();

	return 0;
};