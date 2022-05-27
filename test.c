#include <stdio.h>
#define putd(x) printf(#x ": %d\n", x)


int search_div(int num, int denom, int postest) {
  if(num >= 0 && postest * denom >= num) {
    return  postest;
  } else if( num <= 0 && (-postest) * denom <= num) {
    return -postest;
  } else {
    return search_div(num, denom, postest+1);
  }
}
int away_div(int num, int denom) {
  return search_div(num, denom, 0);
}
int chocolate_boxes(int caramel, int almond, int mint, int nut_free,
    int box_size) {
  int temp;
  int num_boxes = 0;

  temp = caramel / box_size;
  caramel -= temp * box_size;
  num_boxes += temp;

  temp = almond / box_size;
  almond -= temp * box_size;
  num_boxes += temp;


  temp = mint / box_size;
  mint -= temp * box_size;
  num_boxes += temp;


  num_boxes += away_div(nut_free, box_size);
  putd(nut_free);
  putd(box_size);
  putd(away_div(nut_free, box_size));

  int half_size = box_size / 2;

  int num_half = 0;
  num_half += away_div(caramel, half_size);
  num_half += away_div(almond, half_size);
  num_half += away_div(mint, half_size);

  num_boxes += away_div(num_half, 2);
  return num_boxes;
}

int main()
{
	printf("%d\n", chocolate_boxes(2,2,2,2,2));
}
