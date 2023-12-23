#include <iostream>
#include <crtdbg.h>

class MyObject {
	std::string* const _name;
public:
	MyObject(std::string name) : _name(new std::string(name)) {
		std::cout << "MyObject(name:\"" << *_name << "\") was created." << std::endl;
	}
	MyObject() : MyObject("(unnamed)") {}
	~MyObject() {
		std::cout << "MyObject(name:\"" << *_name << "\") was deleted." << std::endl;
		delete _name;
	}
	const std::string& name() const { return *_name; }
};

void printMyObject(const MyObject& myObject) {
	std::cout << "MyObject(name:\"" << myObject.name() << "\")" << std::endl;
}

#include <string>

void destructorStudy_1() {
	std::cout << "オブジェクトをスタック上に作成する。オブジェクトはスコープから出る際(関数終了時ではない)に削除される。" << std::endl;
	{
		MyObject stackObject("on stack");
		printMyObject(stackObject);
	}	// スコープの終了。このタイミングでオブジェクトは削除される

	std::cout << "終了" << std::endl << std::endl;
}

void destructorStudy_2() {
	std::cout << "オブジェクトをヒープに作成する。オブジェクトは不要となるタイミングで明にdeleteする必要がある。" << std::endl;
	auto heapObject = new MyObject("in heap");
	printMyObject(*heapObject);
	delete heapObject;

	std::cout << "終了" << std::endl << std::endl;
}

void destructorStudy_3() {
	std::cout << "オブジェクトの配列をスタック上に作成する" << std::endl;

	const auto arraySize = 3;
	{
		MyObject objectArray[arraySize];
		for (auto i = 0; i < arraySize; i++) {
			std::cout << i << " : ";
			printMyObject(objectArray[i]);
		}
	}	// スコープの終了。このタイミングでオブジェクトは削除される

	std::cout << "終了" << std::endl << std::endl;
}

void destructorStudy_4() {
	std::cout << "オブジェクトの配列をヒープに作成する" << std::endl;
	const auto arraySize = 5;
	auto heapObjectArray = new MyObject[arraySize];
	for (auto i = 0; i < arraySize; i++) {
		std::cout << i << " : ";
		printMyObject(heapObjectArray[i]);
	}
	delete[] heapObjectArray;	// これを実行すると、配列の各要素(MyObjectのインスタンス)のデストラクタと、配列用のメモリが解放される。

	std::cout << "終了" << std::endl << std::endl;
}

void destructorStudy_5() {
	std::cout << "オブジェクトのポインター配列をスタックに作成する" << std::endl;
	const auto arraySize = 7;
	MyObject* objectArray[arraySize];
	for (auto i = 0; i < arraySize; i++) {
		if ((i & 1) == 0) {
			// 偶数の場合はMyObjectは生成せず、配列には0(null)をセットする。
			objectArray[i] = 0;
		} else {
			// 奇数の場合はMyObjectを生成し、配列にポインターをセットする。
			objectArray[i] = new MyObject(std::to_string(i));
		}
	}
	for (auto i = 0; i < arraySize; i++) {
		std::cout << i << " : ";
		auto o = objectArray[i];
		if (o == 0) {
			std::cout << "null" << std::endl;
		} else {
			printMyObject(*o);
		}
	}

	// 配列はスタック上にあるので明示的に開放する必要はないが、配列要素のポインタが指すオブジェクトの削除は明示的に行う必要がある
	for (auto i = 0; i < arraySize; i++) {
		std::cout << i << " : ";
		auto o = objectArray[i];
		if (o == 0) {
			std::cout << "null" << std::endl;
		} else {
			delete o;
		}
	}
}

void destructorStudy_6() {
	std::cout << "オブジェクトのポインター配列をヒープに作成する" << std::endl;

	const auto arraySize = 7;

	// オブジェクト等を生成する。
	auto heapObjectArray = new MyObject * [arraySize];
	for (auto i = 0; i < arraySize; i++) {
		if ((i & 1) == 0) {
			// 偶数の場合はMyObjectは生成せず、配列には0(null)をセットする。
			heapObjectArray[i] = 0;
		} else {
			// 奇数の場合はMyObjectを生成し、配列にポインターをセットする。
			heapObjectArray[i] = new MyObject(std::to_string(i));
		}
	}

	// 生成されたオブジェクト等を使用する。
	for (auto i = 0; i < arraySize; i++) {
		std::cout << i << " : ";
		auto o = heapObjectArray[i];
		if (o == 0) {
			std::cout << "null" << std::endl;
		} else {
			printMyObject(*o);
		}
	}

	// オブジェクト等を破棄する
	// 配列も後で明示的に削除する必要があるが、配列要素のポインタが指すオブジェクトの削除も明示的に行う必要がある
	for (auto i = 0; i < arraySize; i++) {
		std::cout << i << " : ";
		auto o = heapObjectArray[i];
		if (o == 0) {
			std::cout << "null" << std::endl;
		} else {
			delete o;
		}
	}

	// これを実行しても、配列用のメモリが解放されるだけで、
	// 配列の各要素が指すMyObjectのデストラクタが呼ばれることはないということに注意。
	// これを実行しなければメモリリークになる。
	// ポインタの配列なので、要素数が少ないとメモリ使用量もごくわずかなので、リークしてもアプリの動作に影響がほとんどないので、忘れると問題個所を見つけるのが難しい。
	delete[] heapObjectArray;
}

void uniquePointerStudy() {
	std::cout << std::endl << "unique_ptrの学習。ヒープではなくスタックに作成したオブジェクトのように、スコープから外れる際に削除を行ってくれる。" << std::endl;

	// おそらく基本的な使い方。単純なポインタと異なり、スコープから外れる際に自動的にdeleteを呼んでくれる。
	{
		std::unique_ptr<MyObject> up1(new MyObject("1"));
	}

	// reset()で同じunique_ptr変数を使いまわすことができる。(あまり使わない?)
	std::unique_ptr<MyObject> up2(new MyObject("old2"));
	up2.reset(new MyObject("new2"));	// 新しいオブジェクトで置き換える。この時古いオブジェクトがリークすることはなく、きちんと古いオブジェクトのdeleteも呼んでくれる。

	std::cout << "終了" << std::endl << std::endl;
}

void sharedPointerStudy() {
	std::cout << std::endl << "shared_ptrの学習。" << std::endl;

	// おそらく基本的な使い方。単純なポインタと異なり、スコープから外れる際に、対象オブジェクトの共有状態をチェックし、自分が唯一の所有者であった場合のみ自動的にdeleteを呼んでくれる。
	{
		std::shared_ptr<MyObject> sp1(new MyObject("unique myobject"));
		{
			std::shared_ptr<MyObject> sp2;
			sp2 = sp1;	// sp1とsp2で同じMyObjectを共有する。
			std::cout << "sp1 : ";
			printMyObject(*sp1);
			std::cout << "sp2 : ";
			printMyObject(*sp2);
		}// ここでsp2がスコープから外れるが、sp1がMyObjectを所有しているので、MyObjectのdeleteは行われない。
		std::cout << "sp1 : ";
		printMyObject(*sp1);
	}// ここでsp1がスコープから外れる。sp1のみがMyObjectを所有していたので、MyObjectのdeleteが行われる。

	// 少し特殊な使い方。所有権の放棄を明示的に行う。
	{
		std::shared_ptr<MyObject> sp1(new MyObject("unique myobject"));
		std::shared_ptr<MyObject> sp2;
		sp2 = sp1;	// sp1とsp2で同じMyObjectを共有する。
		printMyObject(*sp1);
		std::cout << "sp2 : ";
		printMyObject(*sp2);
		std::cout << "sp1 : ";

		std::cout << "sp1の所有権を放棄します。" << std::endl;
		sp1.reset();// ここでsp1がMyObjectの所有権を放棄したが、まだsp2がMyObjectを所有しているので、MyObjectのdeleteは行われない。
		std::cout << "sp1の所有権を放棄しました。" << std::endl;
		std::cout << "sp2の所有権を放棄します。" << std::endl;
		sp2.reset();	// ここでsp2がMyObjectの所有権を放棄した。sp2のみがMyObjectを所有していたので、MyObjectのdeleteが行われる。
		std::cout << "sp2の所有権を放棄しました。" << std::endl;
	}

	std::cout << "終了" << std::endl << std::endl;
}

int main() {
	// メモリリーク検出を有効化(もしメモリリークしたままプログラムが終了したら、リークしている旨をデバッグコンソールに出力してくれる）
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	//destructorStudy_6();
	uniquePointerStudy();
	sharedPointerStudy();
}
