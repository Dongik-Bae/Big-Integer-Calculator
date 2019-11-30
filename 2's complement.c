#include <stdio.h>
#include <stdlib.h>

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

int add256(int256_t *result, const int256_t *op1, const int256_t *op2);
int sub256(int256_t *result, const int256_t *op1, const int256_t *op2);
int mul256(int256_t *result, const int256_t *op1, const int256_t *op2);
int cmp256(const int256_t *op1, const int256_t *op2);

void toggle(int256_t *result, const int256_t *obj);
void copy(int256_t *result, const int256_t *obj);

// 2^32 = 4294967296, 2^31 = 2147483648
int main()
{
	int256_t result, op1, op2;

	// input region
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

	// calculation region	
//	int temp = add256(&result, &op1, &op2);
//	int temp = sub256(&result, &op1, &op2);
	int temp = mul256(&result, &op1, &op2);
//	int temp = cmp256(&op1, &op2);


	// print region
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
	
	printf("cmp : %d\n", temp);

	/*
	printf("op1.[~] = ");
	for(unsigned int filter = 0x80000000, offset = 1, i = 0; i < 31; i++)
	{
		printf("%u", (op1.bit.body_5 & filter) / filter);
		filter = filter >> offset;
	}
	printf("%u\n", op1.bit.body_5 & 0x00000001);
	*/


	return 0;	
}

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

void copy(int256_t *result, const int256_t *obj)
{
	for(int i = 0; i < 8; i++) result->w[i] = obj->w[i];
}

int sub256(int256_t *result, const int256_t *op1, const int256_t *op2)
{
	int256_t temp;
	toggle(&temp, op2);
	int sub = add256(result, op1, &temp);

	return 0;
}

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
