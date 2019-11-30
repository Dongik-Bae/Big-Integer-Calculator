# Big-Integer-Calculator

## 0. 개요

256bit singed integer를 덧셈, 뺄셈, 곱셈, 비교하는 프로그램 코드.


## 1. 2의 보수 방식
#### 1) 256비트 정수 크기의 공용체 생성
```c
typedef union
{
	unsigned int w[8];
	struct
	{
		unsigned int body_0 : 32;
		unsigned int body_1 : 32;
		unsigned int body_2 : 32;
		unsigned int body_3 : 32;
		unsigned int body_4 : 32;
		unsigned int body_5 : 32;
		unsigned int body_6 : 32;
		unsigned int body_7 : 31;
		unsigned char head : 1;
	} bit;
} int256_t;
```
부호 없는 정수 8개를 공용체로 합쳐 256비트 사이즈의 정수를 생성한다. 가장 앞의 1비트는 부호를 나타내므로 따로 지정해준다.
#### 2) 정수 입력
```c
int256_t result, op1, op2;

for(int i = 7; i >= 0; i--)
{
	op1.w[i] = 0;
}
for(int i = 7; i >= 0; i--)
{
	op2.w[i] = 0;
}

op1.w[0] = 4;
op2.w[0] = 3;
op1.bit.head = 0;
op2.bit.head = 0;
```
연산에 쓸 두 정수 op1과 op2를 초기화 시킨 후, 원하는 값을 op1과 op2에 입력한다.
#### 3) 연산 선택 및 출력
```c
	int temp = add256(&result, &op1, &op2);
	//int temp = sub256(&result, &op1, &op2);
	//int temp = mul256(&result, &op1, &op2);
	//int temp = cmp256(&op1, &op2);

	printf("op1 : ");
	for(int i = 7; i >= 0; i--)
	{
		printf("%u ", op1.w[i]);
	}
	printf("\n");
	printf("op2 : ");
	for(int i = 7; i >= 0; i--)
	{
		printf("%u ", op2.w[i]);
	}
	printf("\n");
	printf("result : ");
	for(int i = 7; i >= 0; i--)
	{
		printf("%u ", result.w[i]);
	}
	printf("\n");
	
	//printf("cmp : %d\n", temp);
```
4가지 연산 중 한 가지를 선택하여 코드에 둔다.  설정한 op1과 op2의 값과 연산값이 화면에 출력된다.

