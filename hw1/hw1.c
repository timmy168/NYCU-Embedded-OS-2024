#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>

uint8_t seven_seg_signal[7];
uint8_t led_signal[8];

void convert_to_seven_seg_signal(char num)
{
    switch (num)
    {
    case '1':
        memcpy(seven_seg_signal, (uint8_t[]){0, 1, 1, 0, 0, 0, 0}, sizeof(seven_seg_signal));
        break;
    case '2':
        memcpy(seven_seg_signal, (uint8_t[]){1, 1, 0, 1, 1, 0, 1}, sizeof(seven_seg_signal));
        break;
    case '3':
        memcpy(seven_seg_signal, (uint8_t[]){1, 1, 1, 1, 0, 0, 1}, sizeof(seven_seg_signal));
        break;
    case '4':
        memcpy(seven_seg_signal, (uint8_t[]){0, 1, 1, 0, 0, 1, 1}, sizeof(seven_seg_signal));
        break;
    case '5':
        memcpy(seven_seg_signal, (uint8_t[]){1, 0, 1, 1, 0, 1, 1}, sizeof(seven_seg_signal));
        break;
    case '6':
        memcpy(seven_seg_signal, (uint8_t[]){1, 0, 1, 1, 1, 1, 1}, sizeof(seven_seg_signal));
        break;
    case '7':
        memcpy(seven_seg_signal, (uint8_t[]){1, 1, 1, 0, 0, 0, 0}, sizeof(seven_seg_signal));
        break;
    case '8':
        memcpy(seven_seg_signal, (uint8_t[]){1, 1, 1, 1, 1, 1, 1}, sizeof(seven_seg_signal));
        break;
    case '9':
        memcpy(seven_seg_signal, (uint8_t[]){1, 1, 1, 1, 0, 1, 1}, sizeof(seven_seg_signal));
        break;
    default: // 0
        memcpy(seven_seg_signal, (uint8_t[]){1, 1, 1, 1, 1, 1, 0}, sizeof(seven_seg_signal));
        break;
    }
}

void convert_to_led_signal(int num)
{
    switch (num)
    {
    case 1:
        memcpy(led_signal, (uint8_t[]){1, 0, 0, 0, 0, 0, 0, 0}, sizeof(led_signal));
        break;
    case 2:
        memcpy(led_signal, (uint8_t[]){1, 1, 0, 0, 0, 0, 0, 0}, sizeof(led_signal));
        break;
    case 3:
        memcpy(led_signal, (uint8_t[]){1, 1, 1, 0, 0, 0, 0, 0}, sizeof(led_signal));
        break;
    case 4:
        memcpy(led_signal, (uint8_t[]){1, 1, 1, 1, 0, 0, 0, 0}, sizeof(led_signal));
        break;
    case 5:
        memcpy(led_signal, (uint8_t[]){1, 1, 1, 1, 1, 0, 0, 0}, sizeof(led_signal));
        break;
    case 6:
        memcpy(led_signal, (uint8_t[]){1, 1, 1, 1, 1, 1, 0, 0}, sizeof(led_signal));
        break;
    case 7:
        memcpy(led_signal, (uint8_t[]){1, 1, 1, 1, 1, 1, 1, 0}, sizeof(led_signal));
        break;
    case 8:
        memcpy(led_signal, (uint8_t[]){1, 1, 1, 1, 1, 1, 1, 1}, sizeof(led_signal));
        break;
    default: // 0
        memcpy(led_signal, (uint8_t[]){0, 0, 0, 0, 0, 0, 0, 0}, sizeof(led_signal));
        break;
    }
}

int main(int argc, char **argv)
{
    int num = 0;
    int price = 0;
    int distance = 0;
    char signal[15];
    char zero_signal[15]={'0','0','0','0','0','0','0','0','0','0','0','0','0','0','0'};
    memset(signal, '\0', sizeof(signal));

main_menu:
    distance = 0;
    price = 0;
    printf("1. shop list\n");
    printf("2. order\n");
    int select_main_menu;
    scanf("%d", &select_main_menu);
    if (select_main_menu == 1)
    {
        printf("\n");
        goto shop_list;
    }
    else if (select_main_menu == 2)
    {
        printf("\n");
        goto select_shop;
    }
    else
    {
        printf("Please type again.\n");
        printf("\n");
        goto main_menu;
    }

shop_list:
    printf("Dessert shop: 3km\n");
    printf("Baverage shop: 5km\n");
    printf("Dinner: 8km\n");
    getchar(); // Wait for any key press
    getchar(); // Consume the key press
    printf("\n");
    goto main_menu;

select_shop:
    printf("Please choose from 1~3\n");
    printf("1. Dessert shop\n");
    printf("2. Baverage shop\n");
    printf("3. Dinner\n");
    int select_shop;
    scanf("%d", &select_shop);
    printf("\n");
    goto order;

order:
    printf("Please choose from 1~4\n");
    switch (select_shop)
    {
    case 1:
        distance = 3;
        printf("\n");
        goto dessert_order;
        break;
    case 2:
        distance = 5;
        printf("\n");
        goto baverage_order;
        break;
    case 3:
        distance = 8;
        printf("\n");
        goto dinner_order;
        break;
    default:
        break;
    }

dessert_order:
    printf("1. cookie: $60\n");
    printf("2. cake: $80\n");
    printf("3. confirm\n");
    printf("4. cancel\n");
    int select_dessert;
    scanf("%d", &select_dessert);
    switch (select_dessert)
    {
    case 1:
        printf("How many?\n");
        scanf("%d", &num);
        price += (60 * num);
        printf("\n");
        goto dessert_order;
        break;
    case 2:
        printf("How many?\n");
        scanf("%d", &num);
        price += (80 * num);
        printf("\n");
        goto dessert_order;
        break;
    case 3:
        printf("\n");
        goto delivery;
        break;
    case 4:
        printf("\n");
        goto main_menu;
        break;
    default:
        break;
    }

baverage_order:
    printf("1. tea: $40\n");
    printf("2. boba: $70\n");
    printf("3. confirm\n");
    printf("4. cancel\n");
    int select_baverage;
    scanf("%d", &select_baverage);
    switch (select_baverage)
    {
    case 1:
        printf("How many?\n");
        scanf("%d", &num);
        price += (40 * num);
        printf("\n");
        goto baverage_order;
        break;
    case 2:
        printf("How many?\n");
        scanf("%d", &num);
        price += (70 * num);
        printf("\n");
        goto baverage_order;
        break;
    case 3:
        printf("\n");
        goto delivery;
        break;
    case 4:
        printf("\n");
        goto main_menu;
        break;
    default:
        break;
    }

dinner_order:
    printf("1. fried rice: $120\n");
    printf("2. egg-drop soup: $50\n");
    printf("3. confirm\n");
    printf("4. cancel\n");
    int select_dinner;
    scanf("%d", &select_dinner);
    switch (select_dinner)
    {
    case 1:
        printf("How many?\n");
        scanf("%d", &num);
        price += (120 * num);
        printf("\n");
        goto dinner_order;
        break;
    case 2:
        printf("How many?\n");
        scanf("%d", &num);
        price += (50 * num);
        printf("\n");
        goto dinner_order;
        break;
    case 3:
        printf("\n");
        goto delivery;
        break;
    case 4:
        printf("\n");
        goto main_menu;
        break;
    default:
        break;
    }

delivery:
    printf("Delivery Distance: %dkm\n", distance);
    printf("Total Price: %d\n", price);
    printf("Please wait for few minute...\n");
    char distance_str[10];
    char price_str[10];
    sprintf(distance_str, "%d", distance);
    sprintf(price_str, "%d", price);
    int pricelen = strlen(price_str) - 1;
    int price_index = 0;
    
    // writing and reading
    int fd;
    fd = open("/dev/etx_device", O_WRONLY);

    if (fd < 0) {
         printf("Error opening device file!\n");
         return 1;
    }

    int clock = 0;
    while(distance!=0)
    {
        if(price_index > pricelen)
        {
            price_index = 0;
        }
        convert_to_seven_seg_signal(price_str[price_index]);
        convert_to_led_signal(distance);
        printf("-----------------------------------\n");
        printf("Price Digit: %c ", price_str[price_index]);
        printf("Disdantce: %d", distance);
        printf("\n");
        for(int i = 0; i < 7; i++)
        {
            signal[i] = seven_seg_signal[i] ? '1' : '0';
        }
        for(int i = 0; i < 8; i++)
        {
            signal[i + 7] = led_signal[i] ? '1' : '0';
        }

        // print signal
        for(int i = 0; i < 15; i++)
        {
            printf("%c ", signal[i]);
        }
        printf("\n");
        price_index++;
        if(clock == 1) distance--;
        write(fd, signal, 15);
        if(clock == 1) clock = 0;
        else if(clock == 0) clock = 1;
        usleep(500000);
    }

    write(fd, zero_signal, 15);
    close(fd);

    printf("Please pick up your meal.\n");
    getchar();
    getchar();
    goto main_menu;

    return 0;
}
