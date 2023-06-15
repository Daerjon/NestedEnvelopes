#include <algorithm>
#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include <random>
#include <chrono>
#include <filesystem>
namespace fs = std::filesystem;

class box {
public:
    int w;
    int l;

    box(int a, int b) :w(std::max(a, b)), l(std::min(a, b)) {};

    bool canContain(const box& inside)
    {
        return l > inside.l && w > inside.w;
    }

    friend std::ostream& operator<< (std::ostream& stream, const box& b) {
        stream << b.w << " " << b.l;
        return stream;
    }
};

const static bool BoxLesserWidthGreaterLength(const box& lhs, const box& rhs)
{
    return lhs.w == rhs.w ? lhs.l > rhs.l : lhs.w < rhs.w;
}

bool BoxLesserLength(const box& lhs, const box& rhs)
{
    return lhs.l <= rhs.l;
}

int BinarySearch(std::vector<box> vec, box b)
{
    int n = vec.size();
    int l = 0;
    int h = n;
    while (l < h) {
        int mid = l + (h - l) / 2;
        if (BoxLesserLength(b,vec[mid])) 
        {
            h = mid;
        }
        else 
        {
            l = mid + 1;
        }
    }
    return l;
}

std::vector<box> mergeSort(std::vector<box> vec) 
{
    if (vec.size() > 1) {
        int mid = vec.size() / 2;
        std::vector<box> lefthalf(vec.begin(), vec.begin() + mid);
        std::vector<box> righthalf(vec.begin() + mid, vec.begin() + vec.size());

        lefthalf = mergeSort(lefthalf);
        righthalf = mergeSort(righthalf);

        unsigned i = 0;
        unsigned j = 0;
        unsigned k = 0;
        while (i < lefthalf.size() && j < righthalf.size()) {
            if (BoxLesserWidthGreaterLength(lefthalf[i], righthalf[j])) {
                vec[k] = lefthalf[i];
                i++;
            }
            else {
                vec[k] = righthalf[j];
                j++;
            }
            k++;
        }
        while (i < lefthalf.size()) {
            vec[k] = lefthalf[i];
            i++;
            k++;
        }

        while (j < righthalf.size()) {
            vec[k] = righthalf[j];
            j++;
            k++;
        }

    }
    return vec;
}

void SaveToFile(std::string s, std::vector<box> vec)
{
    std::ofstream File(s);
    File << vec.size() << "\n";
    for (int i = 0; i < vec.size(); i++)
    {
        File << vec[i] << "\n";
    }
}

std::vector<box> LoadFromFile(std::string s)
{
    std::ifstream File(s);
    int size;
    File >> size;
    std::vector<box> vec;
    for (int i = 0; i < size; i++)
    {
        int w, l;
        File >> w >> l;
        vec.push_back(box(w, l));
    }
    return vec;
}

std::vector<box> nested_boxes(
    std::vector<box>& arr)
{
    int N = arr.size();

    if (N == 0)
        return std::vector<box>();

    arr = mergeSort(arr);
    int* P = new int[N];
    std::vector<box> X;
    std::vector<int> Z;

    for (int i = 0; i < N; ++i) {
        P[i] = -1;
        int index = BinarySearch(X, arr[i]);

        if (index == X.size() || 0 == X.size())
        {
            X.push_back(arr[i]);
            Z.push_back(i);
        }
        else if (arr[i].l < X[index].l)
        {
            X[index] = arr[i];
            Z[index] = i;
        }
        if (index - 1 >= 0)
            P[i] = Z[index - 1];
    }

    std::vector<box> ret;

    for (int i = Z[X.size() - 1]; i != -1; i = P[i])
        ret.push_back(arr[i]);

    return ret;
}

void DoTests(int minsize, int maxsize, int cases, int step, char format)
{
    int sizelen = (maxsize - minsize)/step+1;
    long long* res = new long long[sizelen];
    float* maxlen = new float[sizelen];
    float* maxwid = new float[sizelen];
    float* reslen = new float[sizelen];
    std::vector<std::vector<box>> tests;
    for (int i = 0; i < sizelen; i ++)
    {
        std::vector<std::vector<box>> results;
        maxlen[i] = 0.0f;
        maxwid[i] = 0.0f;
        reslen[i] = 0.0f;
        tests.clear();
        tests.resize(cases);
        int size = minsize + i * step;
        auto createTest = [size]() -> std::vector<box> {
            std::random_device rd;
            std::mt19937 gen(rd());
            std::uniform_int_distribution<> distrib(1, size);
            std::vector<box> test;
            for (int j = 0; j < size; j++)
            {
                test.push_back(box(distrib(gen), distrib(gen)));
            }
            return test;
        };
        std::generate(tests.begin(), tests.end(), createTest);
        auto start = std::chrono::high_resolution_clock::now();
        for (auto test : tests)
        { 
            results.push_back(nested_boxes(test));
        }
        auto stop = std::chrono::high_resolution_clock::now();
        res[i] = duration_cast<std::chrono::microseconds>(stop - start).count() / 100;
        for (auto r : results)
        {
            maxlen[i] += r[0].l;
            maxwid[i] += r[0].w;
            reslen[i] += r.size();
        }
        maxlen[i] /= static_cast<float>(cases);
        maxwid[i] /= static_cast<float>(cases);
        reslen[i] /= static_cast<float>(cases);
    }

    if (format == 'd')
    {
        std::cout << "czas:\n";
        for (int i = 0; i < sizelen; i++)
        {
            std::cout << "(" << minsize+i*step << "," << res[i] << ")" << "\n";
        }
        std::cout << "dlugosc:\n";
        for (int i = 0; i < sizelen; i++)
        {
            std::cout << "(" << minsize + i * step << "," << maxlen[i] << ")" << "\n";
        }
        std::cout << "szerokosc:\n";
        for (int i = 0; i < sizelen; i++)
        {
            std::cout << "(" << minsize + i * step << "," << maxwid[i] << ")" << "\n";
        }
        std::cout << "rozmiar:\n";
        for (int i = 0; i < sizelen; i++)
        {
            std::cout << "(" << minsize + i * step << "," << reslen[i] << ")" << "\n";
        }
    }
    free(maxlen);
    free(reslen);
    free(maxwid);
}

void DoSizeTests(int minsize, int maxsize, int cases, int size, int step, char format)
{
    int sizelen = (maxsize - minsize) / step + 1;
    long long* res = new long long[sizelen];
    float* maxlen = new float[sizelen];
    float* maxwid = new float[sizelen];
    float* reslen = new float[sizelen];
    std::vector<std::vector<box>> tests;
    for (int i = 0; i < sizelen; i++)
    {
        std::vector<std::vector<box>> results;
        maxlen[i] = 0.0f;
        maxwid[i] = 0.0f;
        reslen[i] = 0.0f;
        tests.clear();
        tests.resize(cases);
        int wid = minsize + i * step;
        auto createTest = [size, wid]() -> std::vector<box> {
            std::random_device rd;
            std::mt19937 gen(rd());
            std::uniform_int_distribution<> distrib(1, wid);
            std::vector<box> test;
            for (int j = 0; j < size; j++)
            {
                test.push_back(box(distrib(gen), distrib(gen)));
            }
            return test;
        };
        std::generate(tests.begin(), tests.end(), createTest);
        auto start = std::chrono::high_resolution_clock::now();
        for (auto test : tests)
        {
            results.push_back(nested_boxes(test));
        }
        auto stop = std::chrono::high_resolution_clock::now();
        res[i] = duration_cast<std::chrono::microseconds>(stop - start).count() / 100;
        for (auto r : results)
        {
            maxlen[i] += r[0].l;
            maxwid[i] += r[0].w;
            reslen[i] += r.size();
        }
        maxlen[i] /= static_cast<float>(cases);
        maxwid[i] /= static_cast<float>(cases);
        reslen[i] /= static_cast<float>(cases);
    }

    if (format == 'd')
    {
        std::cout << "czas:\n";
        for (int i = 0; i < sizelen; i++)
        {
            std::cout << "(" << minsize + i * step << "," << res[i] << ")" << "\n";
        }
        std::cout << "dlugosc:\n";
        for (int i = 0; i < sizelen; i++)
        {
            std::cout << "(" << minsize + i * step << "," << maxlen[i] << ")" << "\n";
        }
        std::cout << "szerokosc:\n";
        for (int i = 0; i < sizelen; i++)
        {
            std::cout << "(" << minsize + i * step << "," << maxwid[i] << ")" << "\n";
        }
        std::cout << "rozmiar:\n";
        for (int i = 0; i < sizelen; i++)
        {
            std::cout << "(" << minsize + i * step << "," << reslen[i] << ")" << "\n";
        }
    }
    free(maxlen);
    free(reslen);
    free(maxwid);
}

int main(int argc, char* argv[])
{
    if (argc == 1)
    {
        std::cout << "Niepoprawne uzycie\n Aby uzyc aplikacji musisz wpisac w terminalu \n .\\nestedBoxes nazwy plikow wejsciowych\n";
        std::cout << "W nazwach plikow wejsciowych mozna podac dowolna ilosc plikow\n nalezy podac nazwe wraz z rozszerzeniem np. test.txt\n";
        return 0;
    }

    if ((argc == 2) && (std::string(argv[1]) == "-a"))
    {
        std::filesystem::path cwd = std::filesystem::current_path();
        std::string path = cwd.generic_string();
        for (const auto& entry : fs::directory_iterator(path))
        {
            std::string ext = entry.path().extension().generic_string();
            if (entry.path().extension().generic_string() == ".txt")
            {
                std::string input(entry.path().filename().generic_string());
                std::string output = input.substr(0, input.find_last_of('.')) + "_result.txt";
                std::vector<box> b = LoadFromFile(std::string(input));
                std::vector<box> ans = nested_boxes(b);
                SaveToFile(output, ans);
            }
        }
        return 0;
    }

    if ((argc == 7) && (std::string(argv[1]) == "-t"))
    {
        int minsize = std::stoi(argv[2]);
        int maxsize = std::atoi(argv[3]);
        int cases = std::atoi(argv[4]);
        int step = std::atoi(argv[5]);
        char format = argv[6][0];
        DoTests(minsize, maxsize, cases, step, format);
        return 0;
    }

    if ((argc == 8) && (std::string(argv[1]) == "-s"))
    {
        int minsize = std::stoi(argv[2]);
        int maxsize = std::atoi(argv[3]);
        int cases = std::atoi(argv[4]);
        int size = std::atoi(argv[5]);
        int step = std::atoi(argv[6]);
        char format = argv[7][0];
        DoSizeTests(minsize, maxsize, cases, size, step, format);
        return 0;
    }

    if ((argc == 5) && (std::string(argv[1]) == "-g"))
    {
        int a, b, c;
        a = std::stoi(argv[2]);
        b = std::stoi(argv[3]);
        c = std::stoi(argv[4]);
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> distrib(1, c);
        for (int i = 0; i < a; i++)
        {
            std::vector<box> test;
            for (int j = 0; j < b; j++)
            {
                test.push_back(box(distrib(gen), distrib(gen)));
            }
            SaveToFile("test_" + std::to_string(i) + ".txt", test);
        }
        return 0;
    }

    for (int i = 1; i < argc; i++)
    {
        std::string input(argv[i]);
        std::string output = input.substr(0, input.find_last_of('.')) + "_result.txt";
        std::vector<box> b = LoadFromFile(std::string(input));
        std::vector<box> ans = nested_boxes(b);
        SaveToFile(output, ans);
    }

    return 0;
}