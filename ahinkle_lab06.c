#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define LINESIZE 1024

struct listing {
    int id, host_id, minimum_nights, number_of_reviews,
        calculated_host_listings_count, availability_365;

    char *host_name, *neighbourhood_group,
         *neighbourhood, *room_type;

    float latitude, longitude, price;
};

struct listing getfields(char *line) {
    struct listing item;

    item.id = atoi(strtok(line, ","));
    item.host_id = atoi(strtok(NULL, ","));
    item.host_name = strdup(strtok(NULL, ","));
    item.neighbourhood_group = strdup(strtok(NULL, ","));
    item.neighbourhood = strdup(strtok(NULL, ","));
    item.latitude = atof(strtok(NULL, ","));
    item.longitude = atof(strtok(NULL, ","));
    item.room_type = strdup(strtok(NULL, ","));
    item.price = atof(strtok(NULL, ","));
    item.minimum_nights = atoi(strtok(NULL, ","));
    item.number_of_reviews = atoi(strtok(NULL, ","));
    item.calculated_host_listings_count = atoi(strtok(NULL, ","));
    item.availability_365 = atoi(strtok(NULL, ","));

    return item;
}

/* Compares two listings alphabetically by host name. */
int compareHostName(const void *a, const void *b) {
    const struct listing *first = a;
    const struct listing *second = b;

    return strcmp(first->host_name, second->host_name);
}

/* Compares two listings numerically by price. */
int comparePrice(const void *a, const void *b) {
    const struct listing *first = a;
    const struct listing *second = b;

    if (first->price < second->price) {
        return -1;
    }

    if (first->price > second->price) {
        return 1;
    }

    return 0;
}

/* Writes sorted structures to new csv file */
void writeFile(char *filename, struct listing items[], int count) {
    FILE *output;
    int i;

    output = fopen(filename, "w");

    if (output == NULL) {
        fprintf(stderr, "Error opening %s\n", filename);
        return;
    }

    for (i = 0; i < count; i++) {
        fprintf(
            output,
            "%d,%d,%s,%s,%s,%f,%f,%s,%f,%d,%d,%d,%d\n",
            items[i].id,
            items[i].host_id,
            items[i].host_name,
            items[i].neighbourhood_group,
            items[i].neighbourhood,
            items[i].latitude,
            items[i].longitude,
            items[i].room_type,
            items[i].price,
            items[i].minimum_nights,
            items[i].number_of_reviews,
            items[i].calculated_host_listings_count,
            items[i].availability_365
        );
    }

    fclose(output);
}

int main(void) {
    struct listing list_items[22555];
    char line[LINESIZE];
    int count = 0;

    FILE *fptr = fopen("listings.csv", "r");

    if (fptr == NULL) {
        fprintf(stderr, "Error reading listings.csv\n");
        return 1;
    }

    while (fgets(line, LINESIZE, fptr) != NULL) {
        list_items[count++] = getfields(line);
    }

    fclose(fptr);

    /* Sorts structures by host name */
    qsort(
        list_items,
        count,
        sizeof(struct listing),
        compareHostName
    );

    writeFile(
        "sorted_host_name.csv",
        list_items,
        count
    );

    /* Sort by price */
    qsort(
        list_items,
        count,
        sizeof(struct listing),
        comparePrice
    );

    writeFile(
        "sorted_price.csv",
        list_items,
        count
    );

    printf("Created sorted_host_name.csv\n");
    printf("Created sorted_price.csv\n");

    return 0;
}