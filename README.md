# Big-Integer-Calculator

## 0. 개요

256bit singed integer를 덧셈, 뺄셈, 곱셈, 비교하는 프로그램 코드.


## 1. 2의 보수 방식
##### 1) 256비트 정수 크기의 공용체 생성
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
##### 2) 정수 입력
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
#### 4) 덧셈 함수
```c
int add256(int256_t *result, const int256_t *op1, const int256_t *op2)
{
	unsigned int carry = 0;
	unsigned long long temp = 4294967296;
	unsigned long long save = 0;
	
	for(int i = 0; i < 7; i++)
	{	
		long long cal = (long long)(op1->w[i]) + (long long)(op2->w[i]) + (long long)(carry);
		if(cal >= temp)
		{	
			carry = 1;
			result->w[i] = (int)(cal - temp); 
		}		
		else
		{
			carry = 0;	
			result->w[i] = (int)(cal);
		}
	}

	save = (long long)(op1->bit.body_7) + (long long)(op2->bit.body_7) + (long long)(carry);
	if((op1->bit.head == 0) && (op2->bit.head == 0))
	{
		if(save < temp/2)
		{
			result->bit.body_7 = (int)(save);
			result->bit.head = 0;
		}
		else printf("\n\n\nOVERFLOW!\n\n\n\n");	
	}
	else if((op1->bit.head == 1) && (op2->bit.head == 1))
	{
		if(save < temp/2) printf("\n\n\nOVERFLOW!\n\n\n\n");
		else
		{
			result->bit.body_7 = (int)(save - temp/2);
			result->bit.head = 1;
		}
	}
	else
	{
		if(save < temp/2)
		{
			result->bit.body_7 = (int)(save);
			result->bit.head = 1;
		}
		else
		{
			result->bit.body_7 = (int)(save - temp/2);	
			result->bit.head = 0;
		}
	}
	return 0;	
}
```
입력 받은 두 정수를 덧셈하는 함수이다. 한 body의 연산 결과값이 32비트를 초과할 경우, 다음 body에 carry가 생성되어 넘어간다. 가장 큰 8비트는 부호를 포함하고 있으므로 7비트만 덧셈한다.
#### 5) 음수화 함수
```c
void toggle(int256_t *result, const int256_t *obj)
{
	int256_t plus1, temp1, temp2;
	for(int i = 7; i >= 0; i--)
	{
		plus1.w[i] = 0;
		temp2.w[i] = 0;
	}
	plus1.w[0] = 1;

	for(int i = 7; i >= 0; i--) temp1.w[i] = obj->w[i] ^ 0xffffffff;

	int temp = 0, count = 0;
	for(int i = 0; i < 8; i++) if(obj->w[i] == 0) count++;
	if(count != 8) temp = add256(&temp2, &temp1, &plus1);

	copy(result, &temp2);
}
```
입력 받은 정수를 음수로 변화시킨다. 2의 보수 체계에서는 정수의 값을 전부 토글시킨 다음 1을 더해주어야 한다.
#### 6) 복사 함수
```c
void copy(int256_t *result, const int256_t *obj)
{
	for(int i = 0; i < 8; i++) result->w[i] = obj->w[i];
}
```
입력한 정수를 다른 공용체에 저장한다.

#### 7) 뺄셈 함수
```c
int sub256(int256_t *result, const int256_t *op1, const int256_t *op2)
{
	int256_t temp;
	toggle(&temp, op2);
	int sub = add256(result, op1, &temp);

	return 0;
}
```
한 정수를 음수화 한 다음 다른 정수와 덧셈한다.
#### 8) 곱셈 함수
```c
int mul256(int256_t *result, const int256_t *op1, const int256_t *op2)
{
	int toggle_count = 0;
	int256_t temp1, temp2, temp3, temp4, temp0;
	for(int i = 7; i >= 0; i--)
	{
		temp0.w[i] = 0;
		temp3.w[i] = 0;
		temp4.w[i] = 0;
	}

	if(op1->bit.head == 1)
	{
		toggle_count++;
		toggle(&temp1, op1);	
	}
	else copy(&temp1, op1);
	if(op2->bit.head == 1)
	{
		toggle_count++;
		toggle(&temp2, op2);	
	}
	else copy(&temp2, op2);
	
	unsigned long long mulbox = 0;
	int temp = 0, of = 0;
	for(int i = 0; i < 8; i++)
	{
		for(int j = 0; i + j < 8; j++)
		{
			mulbox = (long long)(temp1.w[j]) * (long long)(temp2.w[i]);
			if(i + j < 8) temp3.w[i + j] += mulbox & 0xffffffff;
			if(i + j < 7) temp3.w[i + j + 1] += (mulbox >> 32) & 0x8fffffff;
			
			if((mulbox >= 2147483648) && (i + j == 7)) of++;
			else if((mulbox >= 0) && (i + j >= 8)) of++;
			else temp = add256(&temp4, &temp4, &temp3);
			copy(&temp3, &temp0);
		}	
	}
	if(of > 0) printf("\n\n\nOVERFLOW!\n\n\n\n");
	temp4.bit.head = 0;
	if(toggle_count % 2 == 1) toggle(result, &temp4);
	else copy(result, &temp4);
	return 0;
}
```
각각의 body끼리 곱셈을 연산하고 overflow 되는 양은 다음 body에서 처리한다.

#### 9) 비교 함수
```c
int cmp256(const int256_t *op1, const int256_t *op2)
{
	int check = 0, temp = -1, temp0 = 0;
	int head1 = op1->bit.head, head2 = op2->bit.head;

	if((op1->bit.body_7 == op2->bit.body_7) && (op1->bit.body_7 == 0)) temp0++;
	for(int i = 6; i >= 0; i--)
	{
		if(op1->w[i] == op2->w[i])
		{
			if(op1->w[i] == 0) temp0++;
		}
		else
		{
			temp = i;
			break;
		}
	}

	if(head1 != head2)
	{
		if((temp == -1) && (temp0 == 8)) check = 0;
		else if(head1 == 1) check = -1;
		else check = 1;
	}
	else
	{
		if(op1->bit.body_7 > op2->bit.body_7) check = 1;
		else if(op1->bit.body_7 < op2->bit.body_7) check = -1;
		else
		{
			if(op1->w[temp] > op2->w[temp]) check = 1;
			else if(op1->w[temp] < op2->w[temp]) check = -1;
			else check = 0;
		}
		
		if(head1 == 1) check *= -1;
	}

	return check;
}
```
부호 비트, 값이 큰 body 순서로 값을 비교한다. 1이면 op1이 크고, 0이면 같고, -1이면 op2가 크다.
