#include <stdio.h>
#include <stdlib.h>

typedef struct
{
	unsigned int w[8];
	unsigned char s;	
} sint256_t;

int add256(sint256_t *result, const sint256_t *op1, const sint256_t *op2);
int sub256(sint256_t *result, const sint256_t *op1, const sint256_t *op2);
int mul256(sint256_t *result, const sint256_t *op1, const sint256_t *op2);
int cmp256(const sint256_t *op1, const sint256_t *op2);

void toggle(sint256_t *result, const sint256_t *obj);
void copy(sint256_t *result, const sint256_t *obj);
void process_s(sint256_t *result, const sint256_t *op1, const sint256_t *op2);

// 2^32 = 4294967296, 2^31 = 2147483648
int main()
{
	sint256_t result, op1, op2;

	// input region
	for(int i = 7; i >= 0; i--)
	{
		op1.w[i] = 0;
	}
	for(int i = 7; i >= 0; i--)
	{
		op2.w[i] = 0;
	}

	op1.s = '+';
	op2.s = '+';
	op1.w[7] = 2000000000;
	op2.w[7] = 2000000000;


	// calculation region	
//	int temp = add256(&result, &op1, &op2);
//	int temp = sub256(&result, &op1, &op2);
	int temp = mul256(&result, &op1, &op2);
//	int temp = cmp256(&op1, &op2);



	// print region
	if((op1.s != '+') && (op1.s != '-')) printf("\n\n\nERROR\n\n\n\n");
	if((op2.s != '+') && (op2.s != '-')) printf("\n\n\nERROR\n\n\n\n");
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

	printf("sign : %c\n", result.s);
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

int add256(sint256_t *result, const sint256_t *op1, const sint256_t *op2)
{
	unsigned int carry = 0;
	unsigned long long temp = 4294967296;
	unsigned long long save = 0;
	sint256_t temp1, temp2, temp3;
	for(int i = 0; i < 8; i++)
	{
		temp1.w[i] = 0;
		temp2.w[i] = 0;
		temp3.w[i] = 0;
	}
	
	if(op1->s == op2->s)
	{		
		for(int i = 0; i < 7; i++)
		{	
			long long cal = (long long)op1->w[i] + (long long)op2->w[i] + (long long)carry;
			if(cal >= temp)
			{	
				carry = 1;
				temp3.w[i] = (int)(cal - temp); 
			}		
			else
			{
				carry = 0;	
				temp3.w[i] = (int)(cal);
			}
		}

		save = (long long)(op1->w[7]) + (long long)(op2->w[7]) + (long long)(carry);
		if(save < temp) result->w[7] = (int)save;
		else printf("\n\n\nOVERFLOW!\n\n\n\n");		
		temp3.s = op1->s;
	}
	else
	{
		int sign1 = 0, sign2 = 0;
		if(op1->s == '-')
		{
			toggle(&temp1, op1);
			sign1 = 1;	
		}
		else copy(&temp1, op1);
		if(op2->s == '-')
		{
			toggle(&temp2, op2);
			sign2 = 1;
		}
		else copy(&temp2, op2);

		if(cmp256(&temp1, &temp2) == 1)
		{
			process_s(&temp3, &temp1, &temp2);
			if(sign1 == 1) toggle(&temp3, &temp3);
		}
		else
		{
			process_s(&temp3, &temp2, &temp1);
			if(sign2 == 1) toggle(&temp3, &temp3);
		}
	}

	copy(result, &temp3);

	return 0;	
}

void process_s(sint256_t *result, const sint256_t *op1, const sint256_t *op2)
{
	int carry = 0;
	int temp = 0;
	long long sbox = 4294967296;
	result->s = '+';

	for(int i = 0; i < 8; i++)
	{
		temp = op1->w[i] - carry; 
		if(temp < op2->w[i])
		{
			carry = 1;
			result->w[i] = (int)(sbox + (long long)temp - (long long)op2->w[i]);
		}
		else
		{
			carry = 0;
			result->w[i] = temp - op2->w[i];
		}
	}	
}

void toggle(sint256_t *result, const sint256_t *obj)
{
	sint256_t temp;
	for(int i = 7; i >= 0; i--) temp.w[i] = obj->w[i];
	if(obj->s == '+') temp.s = '-';
	else temp.s = '+';

	copy(result, &temp);
}

void copy(sint256_t *result, const sint256_t *obj)
{
	result->s = obj->s;
	for(int i = 0; i < 8; i++) result->w[i] = obj->w[i];
}

int sub256(sint256_t *result, const sint256_t *op1, const sint256_t *op2)
{
	sint256_t temp;
	toggle(&temp, op2);
	int sub = add256(result, op1, &temp);

	return 0;
}

int mul256(sint256_t *result, const sint256_t *op1, const sint256_t *op2)
{
	int toggle_count = 0;
	sint256_t temp1, temp2, temp3, temp4, temp0;
	for(int i = 7; i >= 0; i--)
	{
		temp0.w[i] = 0;
		temp3.w[i] = 0;
		temp4.w[i] = 0;
	}

	if(op1->s == '-')
	{
		toggle_count++;
		toggle(&temp1, op1);	
	}
	else copy(&temp1, op1);
	if(op2->s == '-')
	{
		toggle_count++;
		toggle(&temp2, op2);	
	}
	else copy(&temp2, op2);

	unsigned long long mulbox = 0;
	int temp = 0, of = 0;;
	for(int i = 0; i < 8; i++)
	{
		for(int j = 0; j < 8; j++)
		{
			mulbox = (long long)(temp1.w[j]) * (long long)(temp2.w[i]);
			if(i + j < 8) temp3.w[i + j] += mulbox & 0xffffffff;
			if(i + j < 7) temp3.w[i + j + 1] += (mulbox >> 32) & 0xffffffff;
			
			if((mulbox >= 4294967296) && (i + j == 7)) of++;
			else if((mulbox >= 0) && (i + j >= 8)) of++;
			else temp = add256(&temp4, &temp4, &temp3);			
			copy(&temp3, &temp0);
		}
	}
	if(of > 0) printf("\n\n\nOVERFLOW!\n\n\n\n");
	temp4.s = '+';
	if(toggle_count % 2 == 1) toggle(result, &temp4);
	else copy(result, &temp4);
	return 0;
}


int cmp256(const sint256_t *op1, const sint256_t *op2)
{
	int check = 0, temp = -1, temp0 = 0;;
	int head1 = op1->s, head2 = op2->s;

	
	for(int i = 7; i >= 0; i--)
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
		if ((temp == -1) && (temp0 == 8)) check = 0;
		else if(head1 == '-') check = -1;
		else check = 1;
	}
	else
	{
		if(op1->w[temp] > op2->w[temp]) check = 1;
		else if(op1->w[temp] < op2->w[temp]) check = -1;
		else check = 0;
		
		if(head1 == '-') check *= -1;
	}

	return check;
}
