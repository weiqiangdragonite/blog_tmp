/*
 * 题目：抽签
 *
 * 将写有数字的n个纸片放入口袋中，你可以从口袋中抽取4次纸片，每次记录下纸片上的数字后都将其
 * 放回口袋中。如果这4个数字和是m，就是你赢，否则你输。请编写一个程序，判断当纸片上所写的数字
 * 是k1，k2，k3，...，kn时，是否存在抽取4次和为m的方案。如果存在，输出Yes，否则输出No。
 *
 * 限制条件：
 * 1 <= n <= 50
 * 1 <= m <= 10^8
 * 1 <= ki <= 10^8
 *
 *
 * 样例1
 * 输入： n = 3, m = 10, k = {1, 3, 5}
 * 输出： Yes （例如4次抽取的结果是1，1，3，5，和为10）
 *
 * 样例2
 * 输入： n = 3, m = 9, k = {1, 3, 5}
 * 输出： No （不存在和为9的抽取方案）
 */

#include <stdio.h>

const int MAX_NUM = 50;

int
main(int argc, char *argv[])
{
	int num_of_cards, sum;
	int card[MAX_NUM];
	int index;
	int a, b, c, d;
	char is_found;

	/* Get the info from standard input */
	printf("Please input number of cards: ");
	scanf("%d", &num_of_cards);
	printf("Please input the sum: ");
	scanf("%d", &sum);
	printf("Please input the card number:\n");
	for (index = 0; index < num_of_cards; ++index)
		scanf("%d", &card[index]);

	is_found = 0;
	/* find all the combination through loops
	通过四重循环枚举所有方案 */
	for (a = 0; a < num_of_cards; ++a) {
		for (b = 0; b < num_of_cards; ++b) {
			for (c = 0; c < num_of_cards; ++c) {
				for (d = 0; d < num_of_cards; ++d) {
					if (card[a] + card[b] + card[c] + card[d] == sum)
						is_found = 1;
				}
			}
		}
	}

	printf("\nOutput: ");
	if (is_found)
		printf("Yes\n");
	else
		printf("No\n");


	return 0;
}
