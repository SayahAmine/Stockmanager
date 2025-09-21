#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sqlite3.h"

void create_table(sqlite3 *db) {
    const char *sql = "CREATE TABLE IF NOT EXISTS products ("
                      "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                      "name TEXT NOT NULL,"
                      "category TEXT,"
                      "quantity INTEGER,"
                      "price REAL);";
    char *errMsg = 0;
    sqlite3_exec(db, sql, 0, 0, &errMsg);
}

void add_product(sqlite3 *db) {
    char name[100], category[100];
    int quantity;
    float price;

    printf("Enter product name: ");
    getchar(); // Flush newline
    fgets(name, sizeof(name), stdin);
    name[strcspn(name, "\n")] = 0;

    printf("Enter category: ");
    fgets(category, sizeof(category), stdin);
    category[strcspn(category, "\n")] = 0;

    printf("Enter quantity: ");
    scanf("%d", &quantity);

    printf("Enter price: ");
    scanf("%f", &price);

    char sql[512];
    snprintf(sql, sizeof(sql),
             "INSERT INTO products (name, category, quantity, price) VALUES ('%s', '%s', %d, %.2f);",
             name, category, quantity, price);

    char *errMsg = 0;
    if (sqlite3_exec(db, sql, 0, 0, &errMsg) != SQLITE_OK) {
        printf("Error: %s\n", errMsg);
        sqlite3_free(errMsg);
    } else {
        printf("Product added successfully!\n");
    }
}

void view_products(sqlite3 *db) {
    const char *sql = "SELECT * FROM products;";
    sqlite3_stmt *stmt;

    printf("\nAll Products:\n");
    printf("ID | Name | Category | Quantity | Price\n");
    printf("------------------------------------------\n");

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, 0) == SQLITE_OK) {
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            printf("%d | %s | %s | %d | %.2f\n",
                   sqlite3_column_int(stmt, 0),
                   sqlite3_column_text(stmt, 1),
                   sqlite3_column_text(stmt, 2),
                   sqlite3_column_int(stmt, 3),
                   sqlite3_column_double(stmt, 4));
        }
        sqlite3_finalize(stmt);
    } else {
        printf("Failed to fetch products.\n");
    }
}

void edit_product(sqlite3 *db) {
    int id, quantity;
    float price;
    char name[100], category[100];

    printf("Enter product ID to edit: ");
    scanf("%d", &id);
    getchar(); // Flush newline

    printf("Enter new name: ");
    fgets(name, sizeof(name), stdin);
    name[strcspn(name, "\n")] = 0;

    printf("Enter new category: ");
    fgets(category, sizeof(category), stdin);
    category[strcspn(category, "\n")] = 0;

    printf("Enter new quantity: ");
    scanf("%d", &quantity);
    printf("Enter new price: ");
    scanf("%f", &price);

    char sql[512];
    snprintf(sql, sizeof(sql),
             "UPDATE products SET name='%s', category='%s', quantity=%d, price=%.2f WHERE id=%d;",
             name, category, quantity, price, id);

    char *errMsg = 0;
    if (sqlite3_exec(db, sql, 0, 0, &errMsg) != SQLITE_OK) {
        printf("Error: %s\n", errMsg);
        sqlite3_free(errMsg);
    } else {
        printf("Product updated successfully!\n");
    }
}

void delete_product(sqlite3 *db) {
    int id;
    printf("Enter product ID to delete: ");
    scanf("%d", &id);

    char sql[128];
    snprintf(sql, sizeof(sql), "DELETE FROM products WHERE id=%d;", id);

    char *errMsg = 0;
    if (sqlite3_exec(db, sql, 0, 0, &errMsg) != SQLITE_OK) {
        printf("Error: %s\n", errMsg);
        sqlite3_free(errMsg);
    } else {
        printf("Product deleted successfully!\n");
    }
}

void search_products(sqlite3 *db) {
    char keyword[100];
    printf("Enter keyword to search by name or category: ");
    getchar(); // Flush newline
    fgets(keyword, sizeof(keyword), stdin);
    keyword[strcspn(keyword, "\n")] = 0;

    char sql[256];
    snprintf(sql, sizeof(sql),
             "SELECT * FROM products WHERE name LIKE '%%%s%%' OR category LIKE '%%%s%%';",
             keyword, keyword);

    sqlite3_stmt *stmt;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, 0) == SQLITE_OK) {
        printf("Search Results:\n");
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            printf("%d | %s | %s | %d | %.2f\n",
                   sqlite3_column_int(stmt, 0),
                   sqlite3_column_text(stmt, 1),
                   sqlite3_column_text(stmt, 2),
                   sqlite3_column_int(stmt, 3),
                   sqlite3_column_double(stmt, 4));
        }
        sqlite3_finalize(stmt);
    } else {
        printf("Error searching products.\n");
    }
}

void low_stock_alert(sqlite3 *db) {
    const char *sql = "SELECT * FROM products WHERE quantity < 5;";
    sqlite3_stmt *stmt;
    printf("Low Stock Products (quantity < 5):\n");

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, 0) == SQLITE_OK) {
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            printf("%d | %s | %s | %d | %.2f\n",
                   sqlite3_column_int(stmt, 0),
                   sqlite3_column_text(stmt, 1),
                   sqlite3_column_text(stmt, 2),
                   sqlite3_column_int(stmt, 3),
                   sqlite3_column_double(stmt, 4));
        }
        sqlite3_finalize(stmt);
    } else {
        printf("Error checking low stock.\n");
    }
}

int main() {
    sqlite3 *db;

    if (sqlite3_open("data.db", &db)) {
        fprintf(stderr, "Can't open DB\n");
        return 1;
    }

    create_table(db); // Always ensure table exists

    int choice;
    do {
        printf("\n--- Stock Manager ---\n");
        printf("1. Add Product\n");
        printf("2. View All Products\n");
        printf("3. Edit Product\n");
        printf("4. Delete Product\n");
        printf("5. Search Products\n");
        printf("6. Low Stock Alert\n");
        printf("0. Exit\n");
        printf("Choose an option: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1: add_product(db); break;
            case 2: view_products(db); break;
            case 3: edit_product(db); break;
            case 4: delete_product(db); break;
            case 5: search_products(db); break;
            case 6: low_stock_alert(db); break;
            case 0: printf("Exiting...\n"); break;
            default: printf("Invalid option. Try again.\n");
        }
    } while (choice != 0);

    sqlite3_close(db);
    return 0;
}
