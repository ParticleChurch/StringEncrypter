#include <iostream>
#include <fstream>
#include <random>

inline uint32_t random()
{
	static std::mt19937_64 rng(std::random_device{}());
	return rng() % (uint32_t)-1;
}

constexpr inline char after(char next)
{
	switch (next)
	{
	case '!': return 'X';
	case 'X': return 'O';
	case 'O': return 'R';
	case 'R': return 'd';
	default: return '!';
	}
}

void encryptStrings(char* f, size_t size)
{
	char next = '!';
	size_t d = 0;
	bool dreal = false;
	size_t start = 0;

	for (size_t i = 0; i < size; i++)
	{
		if (next == 'd')
		{
			if ('0' <= f[i] && f[i] <= '9')
			{
				d = d * 10 + f[i] - '0';
				dreal = true;
				continue;
			}
			else if (f[i] == '!')
			{
				char* tag = f + start;
				char* string = f + i + 1;
				if (!dreal) for (d = 1; i + d < size && f[i + d]; d++);

				uint32_t key = random();
				char smallkey = key & 0xff;

				std::cout << "Found string of length: " << d << (dreal ? " (read)" : " (calculated)");
				std::cout << " encoding w/ key=" << key << std::endl;

				size_t offset = 0;
				for (; offset <= d - 4; offset += 4)
					*(uint32_t*)(string + offset) ^= key;
				for (; offset < d; offset ++)
					string[offset] ^= key;

				tag[0] = '^';
				*(uint32_t*)(tag + 1) = key;

				i += d;
				d = 0;
				dreal = false;
				next = '!';
				continue;
			}
			else
			{
				d = 0;
				dreal = false;
				next = '!';
				continue;
			}
		}
		else if (f[i] == next)
		{
			if (next == '!')
			{
				start = i;
			}
			next = after(next);
			continue;
		}
		else
		{
			d = 0;
			dreal = false;
			next = '!';
			continue;
		}
	}
}

int main()
{
	srand(time(0));
	std::fstream ifile;
	std::fstream ofile;
	ifile.open("E:\\GitHub\\temp\\Release\\temp.exe", std::fstream::in | std::fstream::binary);
	ofile.open("E:\\GitHub\\temp\\Release\\out.exe", std::fstream::out | std::fstream::binary);

	if (!ifile.is_open() || !ofile.is_open())
	{
		std::cout << "Failed to open file(s)." << std::endl;
		return 1;
	}

	ifile.seekg(0, std::fstream::end);
	size_t size = ifile.tellg();
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