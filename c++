#define NOMINMAX
#include <iostream>
#include <windows.h>
#include <string>
#include <pqxx/pqxx>
using namespace std;
using namespace pqxx;
connection c("dbname=cinema port=5432 host=localhost user=postgres password = 5258");
work w(c);
void subpay(int* sub, string* pay, result* R);
void timer(int sec, string menu);
int main() {
    try {
        startmenu:
        result R;
        int choice, sub;
        cout << "Welcome, Guest"<<endl;
        cout << "1. Register"<<endl;
        cout << "2. Login"<<endl;
        cout << "3. Exit"<<endl;
        tryNum:
        cout << "make your choice: ";
        cin >> choice;  
        switch (choice) {
        case 1: {
            R = w.exec("SELECT * FROM customers_account");
            result H;
            string login, password, repasw, email, name, lastname, phone, pay;
        tryLogin:
            cout << "Login : ";
            cin >> login;
            for (int i = 0; i != R.size(); i++) {
                if (login.compare(to_string(R[i][5])) == 0) {
                    cout << "This login has already taken by another user."<<endl;
                    goto tryLogin;
                }
            }
            tryPass:
            cout << "Password : ";
            cin >> password;
            cout << "Confirm password : ";
            cin >> repasw;
            if (password.compare(repasw) != 0) {
                cout << "Passwords don't match"<<endl;
                goto tryPass;
            }
            cout << "Your name : ";
            cin >> name;
            cout << "Your last name : ";
            cin >> lastname;
            tryEmail:
            cout << "email : ";
            cin >> email;
            for (int i = 0; i != R.size(); i++) {
                if (email.compare(to_string(R[i][3])) == 0) {
                    cout << "This email has already taken by another user." << endl;
                    goto tryEmail;
                }
            }
            cout << "Your phone number : ";
            cin >> phone;
            subpay(&sub, &pay, &R);
            R = w.exec("SELECT customer_id FROM customers_account");
            w.exec("INSERT INTO customers_account(customer_id, login, password, email, firstname, lastname ,phone_number, subscription_id)"
                " VALUES(" + to_string(R.size() + 1) + " , '" + login + "','" + password + "','" + email + "','" + name + "','" + lastname + "', " + phone + " , "+ to_string(sub) +")"); 
            H = w.exec("SELECT payment_id FROM payment");
            w.exec("INSERT INTO payment(payment_id, payment_method, payment_date, customer_id, subscription_id)"
                " VALUES (" + to_string(H.size() + 1) + ",'" + pay + "',current_date," + to_string(R.size()) + "," + to_string(sub) + ")");
            cout << "You have successfully registered!"<<endl;
            Sleep(1970);
            string menu("start");
            timer(5, menu);
            goto startmenu;
        }
              break;
        case 2: {
        tryLogPass:
            string login, password;
            int attempt=0;
            R = w.exec("SELECT * FROM customers_account");
            cout << "Login : ";
            cin >> login;
            cout << "Password : ";
            cin >> password;
            string admin("admin"), passadm("cheburek");
            if (login.compare(admin) == 0 && password.compare(passadm) == 0) {
                adminmenu:
                cout << "Welcome, Admin"<<endl;
                cout << "      Menu" << endl;
                cout << "1. All Customers" << endl;
                cout << "2. All Movies" << endl;
                cout << "3. Income for all time" << endl;
                cout << "4. income for last month" << endl;
                cout << "5. Add movie " << endl;
                cout << "6. Number of customers on each subscription"<<endl;
                cout << "7. Sign out" << endl;
                cout << "make your choice: ";
                cin >> choice;
                switch (choice) {
                case 1: {
                    cout << endl<< "id|Fname|Lname    |Email   |Phone number | Login| Password| Subcription_ID "<<endl;
                    cout << endl;
                    R = w.exec("SELECT * FROM customers_account");
                    for (auto row : R) {
                        cout << row[0].as<int>() << ' ' << row[1].as<string>() << ' ' << row[2].as<string>() << ' ' << row[3].as<string>() << ' ' << row[4].as<int>() << ' ' << row[5].as<string>() << ' ' << row[6].as<string>() << ' ' << row[7].as<int>() << endl;
                    }
                }
                      break;
                case 2: {
                    R = w.exec("SELECT * FROM movies");
                    cout << "Movie title   | Release   | Rating | Genre    " << endl;
                    for (auto row : R){
                        cout << row[1].as<string>() << " | " << row[2].as<string>() << " | " << row[3].as<string>() << " | " << row[4].as<int>()<< " | " <<row[5].as<string>()<< endl;
                    }
                }
                      break;
                case 3: {
                    R = w.exec("SELECT SUM(price)"
                        " FROM payment JOIN subscription ON payment.subscription_id = subscription.subscription_id ");
                    cout << R[0][0] << " tg";
                }
                      break;
                case 4:
                    R = w.exec("SELECT SUM(price)"
                        " FROM payment"
                        " JOIN subscription ON payment.subscription_id = subscription.subscription_id"
                        " WHERE payment.payment_date >= date_trunc('month', current_date - interval '1' month)");
                    cout << R[0][0] << " tg";
                    break;
                case 5: {
                    R = w.exec("SELECT * FROM movies");
                    string title, date, rate, genre, subs;
                    cout << "Movie title : ";
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    getline(cin, title);
                    cout << "Release date : ";
                    cin >> date;
                    cout << "Rating ?/10 : ";
                    cin >> rate;
                    cout << "Which subscription to send to?" << endl;
                    cout << "1. Ultimate" << endl;
                    cout << "2. Optimal" << endl;
                    cout << "3. Minimal" << endl;
                    cin >> subs;
                    cout << "Genre : " << endl;
                    cin >> genre;
                    w.exec("INSERT INTO movies(movie_id, movie_title, release_date, rating, subscription_id, genre)"
                        " values("+ to_string(R.size() + 1) + ", '" + title + "' , '" + date + "' , '" + rate + "', " + subs + ", '"+ genre +"')");
                    cout << "You have successfully added the movie."<<endl;
                    Sleep(1970);
                }
                      break;
                case 6:
                    R = w.exec("SELECT subscription.subscription_type, COUNT(payment.customer_id)"
                        " FROM payment"
                        " JOIN subscription ON payment.subscription_id = subscription.subscription_id"
                        " GROUP BY subscription.subscription_id");
                    for (auto row : R) {
                        cout << row[0].as<string>() << ' ' << row[1].as<int>() << endl;
                    }
                    break;
                case 7:
                    string menu("start");
                    timer(5, menu );
                    goto startmenu;
                }
                int ch;
                cout << "Which menu to return to?"<<endl;
                cout << "1. Admin" << endl;
                cout << "2. Start" << endl;
                cin >> ch;
                if (ch == 1) {
                    string menu("admin");
                    timer(5, menu);
                    goto adminmenu;
                }
                else {
                    string menu("start");
                    timer(5, menu);
                    goto startmenu;
                }
            }
            else {
                for (int i = 0; i != R.size(); i++) {
                    if (login.compare(to_string(R[i][5])) == 0 && password.compare(to_string(R[i][6])) == 0) {
                    clientmenu:
                        R = w.exec("SELECT * FROM customers_account");
                        cout << "Welcome, " << R[i][1] << endl;
                        cout << "1. My movies" << endl;
                        cout << "2. Change subscription" << endl;
                        cout << "3. Sign out" << endl;
                        cin >> choice;
                        switch (choice) {
                        case 1:
                            R = w.exec("SELECT movie_title, release_date, rating, genre"
                                " FROM movies"
                                " WHERE subscription_id = " + to_string(R[i][7]) + "");
                            for (auto row : R) {
                                cout << row[0].as<string>() << ' ' << row[1].as<string>() << ' ' << row[2].as<string>() << ' ' << row[3].as<string>() << endl;
                            }
                            break;
                        case 2: {
                            string pay;
                            result H;
                            subpay(&sub,&pay ,&R);
                            w.exec("UPDATE customers_account SET subscription_id = "+ to_string(sub) +""
                                " WHERE customer_id = " + to_string(R[i][0]) + "");
                            H = w.exec("SELECT payment_id FROM payment");
                            w.exec("INSERT INTO payment(payment_id, payment_method, payment_date, customer_id, subscription_id)"
                                " VALUES (" + to_string(H.size() + 1) + ",'" + pay + "',current_date," + to_string(R[i][0]) + "," + to_string(sub) + ")");
                            cout << "Your subscription has been changed.";
                            Sleep(1970);
                            string menu("client");
                            timer(5, menu);
                            goto clientmenu;
                        }
                            break;
                        case 3:
                            string menu("start");
                            timer(5, menu);
                            goto startmenu; 
                        }
                        int ch;
                        cout << "Which menu to return to?" << endl;
                        cout << "1. Client" << endl;
                        cout << "2. Start" << endl;
                        cin >> ch;
                        if (ch == 1) {
                            string menu("client");
                            timer(5, menu);
                            goto clientmenu;
                        }
                        else {
                            string menu("start");
                            timer(5, menu);
                            goto startmenu;
                        }
                        
                        break;
                    }
                    else
                        attempt++;
                }
                if (attempt == R.size()) {
                    cout << "Please make sure that you have entered your login and password correctly." << endl;
                    goto tryLogPass;
                }
            }
        }
            break;
        case 3:
            return 0;
        default:
            cout << "Invalid command number" << endl;
            goto tryNum;
        }
        w.commit();  
    }
    catch (const exception& e) {
        cerr << e.what() << endl;
        return 1;
    }
    return 0;
}
void subpay(int* sub, string* pay, result* R) {
submenu:
    result H = *R;
    int choice,payment;
    cout << "Choose your subscription." << endl;
    cout << "1. Minimal" << endl;
    cout << "2. Optimal" << endl;
    cout << "3. Ultimate" << endl;
    cin >> *sub;
    H = w.exec("SELECT price, validity FROM subscription WHERE subscription_id = "+to_string(*sub)+"");
    cout << "Price : " << H[0][0] <<endl;
    cout << "Period : " << H[0][1] << endl;
    H = w.exec("SELECT movie_title, release_date"
        " FROM movies"
        " WHERE subscription_id = "+ to_string(*sub)+"");
    cout << "Movies     |    Release Dates"<<endl;
    for (auto row : H) {
        cout << row[0].as<string>() << " | " << row[1].as<string>() << endl;
    }
    cout << "1. Subscribe" << endl;
    cout << "2. Return to subscribe menu" << endl;
    cin >> choice;
    if (choice == 2) {
        system("cls");
        goto submenu;
    }
    cout << "Payment method : " << endl;
    cout << "1. Card" << endl;
    cout << "2. Cash" << endl;
tryPay:
    cin >> payment;
    switch (payment) {
    case 1:
        *pay = "card";
        break;
    case 2:
        *pay = "cash";
        break;
    default:
        cout << "Invalid command number" << endl;
        goto tryPay;
    }

}
void timer(int sec, string menu) {
    while (true) {
        system("cls");
        cout.width(1);
        cout << "You will be redirected to the "<< menu <<" menu in " << sec << " seconds...";
        Sleep(970);
        if (sec > 1)
            sec--;
        else
            break;
    }
    system("cls");
}
