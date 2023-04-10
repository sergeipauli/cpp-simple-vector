#include <cassert>
#include <iostream>
#include <numeric>

#include "simple_vector.h"

using namespace std;

class TestClass {
    public:
        TestClass(const TestClass& other) = delete;
        TestClass& operator=(const TestClass& other) = delete;

        TestClass() : TestClass(5) {}

        TestClass(size_t num) : num_(num) {}

        TestClass(TestClass&& other) {
            num_ = exchange(other.num_, 0);
        }

        TestClass& operator= (TestClass&& other) {
            num_ = exchange(other.num_, 0);
            return *this;
        }

        size_t GetNum() const {
            return num_;
        }

    private:
        size_t num_;
};

SimpleVector <int> GenerateVector(size_t size) {
    SimpleVector <int> vec(size);

    iota(vec.begin(), vec.end(), 1);

    return vec;
}

void TestTemporaryObjConstructor() {
    cout << "Test with temporary object, copy elision: ";

    const size_t size_num = 1000000;
    SimpleVector <int> moved_vector(GenerateVector(size_num));

    assert(moved_vector.GetSize() == size_num);

    cout << "\tDone!" << endl;
}

void TestTemporaryObjOperator() {
    cout << "Test with temporary object, operator= : ";

    const size_t size_num = 1000000;
    SimpleVector <int> moved_vector;

    assert(moved_vector.GetSize() == 0);
    moved_vector = GenerateVector(size_num);
    assert(moved_vector.GetSize() == size_num);

    cout << "\tDone!" << endl;
}

void TestNamedMoveConstructor() {
    cout << "Test with named object, move constructor: ";

    const size_t size_num = 1000000;
    SimpleVector <int> vector_to_move(GenerateVector(size_num));

    assert(vector_to_move.GetSize() == size_num);

    SimpleVector <int> moved_vector(move(vector_to_move));

    assert(moved_vector.GetSize() == size_num);
    assert(vector_to_move.GetSize() == 0);

    cout << "\tDone!" << endl;
}

void TestNamedMoveOperator() {
    cout << "Test with named object, operator= : ";

    const size_t size_num = 1000000;
    SimpleVector <int> vector_to_move(GenerateVector(size_num));

    assert(vector_to_move.GetSize() == size_num);

    SimpleVector <int> moved_vector = move(vector_to_move);

    assert(moved_vector.GetSize() == size_num);
    assert(vector_to_move.GetSize() == 0);

    cout << "\t\tDone!" << endl;
}

void TestNoncopiableMoveConstructor() {
    cout << "Test noncopiable object, move constructor: ";

    const size_t size_num = 5;
    SimpleVector <TestClass> vector_to_move;

    for (size_t index = 0; index < size_num; ++index) {
        vector_to_move.PushBack(TestClass(index));
    }

    SimpleVector <TestClass> moved_vector = move(vector_to_move);

    assert(moved_vector.GetSize() == size_num);
    assert(vector_to_move.GetSize() == 0);

    for (size_t index = 0; index < size_num; ++index) {
        assert(moved_vector[index].GetNum() == index);
    }

    cout << "\tDone!" << endl;
}

void TestNoncopiablePushBack() {
    cout << "Test noncopiable push back: ";

    const size_t size_num = 5;
    SimpleVector<TestClass> vec;

    for (size_t index = 0; index < size_num; ++index) {
        vec.PushBack(TestClass(index));
    }

    assert(vec.GetSize() == size_num);

    for (size_t index = 0; index < size_num; ++index) {
        assert(vec[index].GetNum() == index);
    }

    cout << "\t\t\tDone!" << endl;
}

void TestNoncopiableInsert() {
    cout << "Test noncopiable insert: ";

    const size_t size_num = 5;
    SimpleVector <TestClass> vec;

    for (size_t index = 0; index < size_num; ++index) {
        vec.PushBack(TestClass(index));
    }

    // в начало
    vec.Insert(vec.begin(), TestClass(size_num + 1));
    assert(vec.GetSize() == size_num + 1);
    assert(vec.begin() -> GetNum() == size_num + 1);

    // в конец
    vec.Insert(vec.end(), TestClass(size_num + 2));
    assert(vec.GetSize() == size_num + 2);
    assert((vec.end() - 1) -> GetNum() == size_num + 2);

    // в середину
    vec.Insert(vec.begin() + 3, TestClass(size_num + 3));
    assert(vec.GetSize() == size_num + 3);
    assert((vec.begin() + 3) -> GetNum() == size_num + 3);

    cout << "\t\t\tDone!" << endl;
}

void TestNoncopiableErase() {
    cout << "Test noncopiable erase: ";

    const size_t size_num = 3;
    SimpleVector <TestClass> vec;

    for (size_t index = 0; index < size_num; ++index) {
        vec.PushBack(TestClass(index));
    }

    auto it = vec.Erase(vec.begin());
    assert(it -> GetNum() == 1);

    cout << "\t\t\tDone!" << endl;
}

int main() {
    //SimpleVector Unit
    {
        TestTemporaryObjConstructor();
        TestTemporaryObjOperator();
        TestNamedMoveConstructor();
        TestNamedMoveOperator();
        TestNoncopiableMoveConstructor();
        TestNoncopiablePushBack();
        TestNoncopiableInsert();
        TestNoncopiableErase();
    }

    return 0;
}
