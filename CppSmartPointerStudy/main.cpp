#include <string>
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
void uniquePointerStudy_1() {
	std::cout << std::endl << "unique_ptrの学習。"
		"ヒープではなくスタックに作成したオブジェクトのように、"
		"スコープから外れる際に指し示すオブジェクトのdeleteを呼んでくれる。"
		<< std::endl;
	{
		std::unique_ptr<MyObject> up1(new MyObject("1"));
	}// ここでup1がスコープ外になるので、up1が指すMyObjectのdeleteも行われる。

	std::cout << "終了" << std::endl << std::endl;
}

void uniquePointerStudy_2() {
	std::cout << std::endl << "unique_ptrの学習。"
		"reset()の使用例。(あまりreset()は使用しないかも。)"
		"reset()で新しいオブジェクトのポインターをセットすると、それまでのオブジェクトのdeleteも呼んでくれる。"
		<< std::endl;
	{
		std::unique_ptr<MyObject> up2(new MyObject("old2"));
		up2.reset(new MyObject("new2"));	// 新しいオブジェクトで置き換える。この時古いオブジェクトがリークすることはなく、きちんと古いオブジェクトのdeleteも呼んでくれる。
	}

	std::cout << "終了" << std::endl << std::endl;
}

void sharedPointerStudy_1() {
	std::cout << std::endl << "shared_ptrの学習。" << std::endl;
	std::cout << "おそらく基本的な使い方。" << std::endl;

	// 単純なポインタと異なり、スコープから外れる際に、対象オブジェクトの共有状態をチェックし、自分が唯一の所有者であった場合のみ自動的にdeleteを呼んでくれる。
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

	std::cout << "終了" << std::endl << std::endl;
}

void sharedPointerStudy_2() {
	std::cout << std::endl << "shared_ptrの学習。" << std::endl;
	std::cout << "少し特殊な使い方。所有権の放棄を明示的に行う。" << std::endl;

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

void sharedPointerStudy_3() {
	std::cout << std::endl << "shared_ptrの学習。" << std::endl;
	std::cout << "誤った使い方。生ポインターを共有してしまう" << std::endl;

	// 一応「/Eha」というC++コンパイルオプションをして指定することで、nullポインターアクセスなどの例外（構造化例外というらしい）をcatchでとらえられるとのこと。
	// ただし、以下の問題があるので、通常のアプリで使うものではなさそう。
	// ・どのような例外が発生したのかはわからない(!!!)
	// ・速度上のペナルティもあるらしい(多分例外発生時ではなく例外が発生しない場合も遅くなるのだと思う)
	// ・リリースビルドだと/Ehaは無効?(デバッガで実行してもキャッチできなかった）
	try {
		// 単純なポインタと異なり、スコープから外れる際に、対象オブジェクトの共有状態をチェックし、自分が唯一の所有者であった場合のみ自動的にdeleteを呼んでくれる。
		{
			auto p = new MyObject("unique myobject");
			std::shared_ptr<MyObject> sp1(p);
			{
				std::shared_ptr<MyObject> sp2(p);	// (誤った使い方)sp1を無視してshared_ptrオブジェクトを生成する。sp1もsp2も自分が唯一の所有者だと勘違いした状態になる。
				std::cout << "sp1 : ";
				printMyObject(*sp1);
				std::cout << "sp2 : ";
				printMyObject(*sp2);
			}// ここでsp2がスコープから外れる。sp2は自分が唯一の所有者と認識しているため、MyObjectのdeleteを行ってしまう。
			//std::cout << "sp1 : ";
			//printMyObject(*sp1);
		}// ここでsp1がスコープから外れる。sp1は自分が唯一の所有者と認識ているため、削除済みのMyObjectのdeleteを行おうとしてしまい、例外が発生してしまう。
	} catch (...) {
		std::cout << "構造化例外発生。" << std::endl;
	}
	std::cout << "終了" << std::endl << std::endl;
}

int main() {
	// メモリリーク検出を有効化(もしメモリリークしたままプログラムが終了したら、リークしている旨をデバッグコンソールに出力してくれる）
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	//destructorStudy_6();
	uniquePointerStudy_1();
	uniquePointerStudy_2();
	//sharedPointerStudy_1();
	//sharedPointerStudy_2();
	//sharedPointerStudy_3();
}
