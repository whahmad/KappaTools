template<typename T>
void printMatrix(T **matrix, const size_t x, const size_t y, const size_t width)
{
	OStreamGuard guard(std::cout);
	for (size_t i = 0; i < x; ++i)
	{
		for (size_t j = 0; j < y; ++j)
			std::cout << std::setw(width) << matrix[i][j] << " ";
		std::cout << std::endl;
	}
	std::cout << std::endl;
}

template<typename T1, typename T2>
std::ostream &operator<<(std::ostream &os, const std::map<T1, T2> &m)
{
	struct MapStrClass
	{
		static std::string pstr(const std::pair<const T1, T2> &x) { return str(x.first) + ": " + str(x.second); }
	};
	return os << "{" << join(", ", strmap(MapStrClass::pstr, m)) << "}";
}

template<typename T>
std::ostream &operator<<(std::ostream &os, const std::set<T> &s)
{
	return os << "{" << join(", ", s) << "}";
}

template<typename T>
std::ostream &operator<<(std::ostream &os, const std::vector<T> &v)
{
	return os << "[" << join(", ", v) << "]";
}