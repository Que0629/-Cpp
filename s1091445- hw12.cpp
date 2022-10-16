#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include<string.h>
using namespace::std;

struct MemberRecord
{
	char email[40];
	char password[24];
	char IDNumber[12];
	char name[12];
	char phone[12];
};

struct Movie
{
	int movieCode;
	int prices[4]; // prices[0]:adult, prices[1]:concession, prices[2]:disability, prices[3]:elderly
	bool dates[9]; // dates[i] is true if and only if the movie is available on i-th date
	bool sessionTimes[17]; // sessionTimes[i] is true if and only if the movie is available on i-th session
	bool occupiedSeats[9][17][8][12]; // occupiedSeats[i][j] is the occupied Seats for all accounts
};                                           // at j-th session time on i-th date

struct BookingInfo
{
	char email[40];
	int movieCode;
	int dateCode;
	int sessionTimeCode;
	int numTickets[4]; // numTickets[0]: the number of adult tickets,
						 // numTickets[1]: the number of concession tickets,
						 // numTickets[2]: the number of disability tickets,
						 // numTickets[3]: the number of elderly tickets
	char seletedSeats[24][4]; // seleted seats for the user with the specified email
};

char hours[17][8] = { "09:00", "10:00", "11:00", "12:00", "13:00", "14:00", "15:00", "16:00", "17:00",
						  "18:00", "19:00", "20:00", "21:00", "22:00", "23:00", "00:00", "01:00" };

void loadMemberInfo(MemberRecord memberDetails[], int& numMembers);
void loadBookingHistories(BookingInfo bookingHistories[], int& numBookings);
void loadMovies(Movie movies[], int& numMovies);
void loadMovieNames(char movieNames[][60], int numMovies);
void loadAvailableDates(char availableDates[][12], int& numDates);

int inputAnInteger(int begin, int end);
void signIn(MemberRecord memberDetails[], int numMembers, Movie movies[], char movieNames[][60], int numMovies,
	BookingInfo bookingHistories[], int& numBookings, char availableDates[][12], int numDates);
bool illegal(char email[], char password[], MemberRecord memberDetails[], int numMembers, int& recordNumber);
void accountInfor(MemberRecord memberDetails[], int numMembers, int recordNumber);

void buyTickets(BookingInfo bookingHistories[], int& numBookings, Movie movies[], char movieNames[][60],
	int numMovies, char availableDates[][12], int numDates, char email[]);
void selectSeats(BookingInfo bookingHistories[], int numBookings, Movie movies[]);

void displaySessionTimes(Movie movies[], char movieNames[][60], int numMovies,
	char availableDates[][12], int numDates);
void display(Movie movies[], BookingInfo bookingHistory);
void displayBookingHistory(BookingInfo bookingHistories[], int numBookings, Movie movies[],
	char movieNames[][60], char availableDates[][12], char email[]);

void newMember(MemberRecord memberDetails[], int& numMembers);
bool existingID(char newIDNumber[], MemberRecord memberDetails[], int& numMembers);
bool existingEmail(char newEmail[], MemberRecord memberDetails[], int& numMembers);

void saveMemberInfo(MemberRecord memberDetails[], int numMembers);
void saveMovies(Movie movies[], int numMovies);
void saveBookingHistories(BookingInfo bookingHistories[], int numBookings);
int main()
{
	MemberRecord memberDetails[100] = {};
	int numMembers = 0;
	loadMemberInfo(memberDetails, numMembers);

	BookingInfo bookingHistories[1000] = {};
	int numBookings = 0;
	loadBookingHistories(bookingHistories, numBookings);

	Movie movies[30] = {};
	int numMovies = 0;
	loadMovies(movies, numMovies);

	char movieNames[30][60] = {};
	loadMovieNames(movieNames, numMovies);

	char availableDates[10][12];
	int numDates;
	loadAvailableDates(availableDates, numDates);

	cout << "Welcome to Vieshow Cinemas Taipei QSquare system\n";

	int choice;

	while (true)
	{
		cout << "\n1. Sign In\n";
		cout << "2. New Member\n";
		cout << "3. End\n";

		do cout << "\nEnter your choice (1~3): ";
		while ((choice = inputAnInteger(1, 3)) == -1);
		cout << endl;
		switch (choice)
		{
		case 1:
			signIn(memberDetails, numMembers, movies, movieNames, numMovies,
				bookingHistories, numBookings, availableDates, numDates);
			break;

		case 2:
			newMember(memberDetails, numMembers);
			cin.ignore();
			break;

		case 3:
			saveMemberInfo(memberDetails, numMembers);
			saveMovies(movies, numMovies);
			saveBookingHistories(bookingHistories, numBookings);
			cout << "Thank you...\n\n";
			system("pause");
			return 0;

		default:
			cout << "Input Error!\n\n";
			break;
		}
	}

	system("pause");
}


void loadMemberInfo(MemberRecord memberDetails[], int& numMembers) {
	ifstream inMemberFile;
	inMemberFile.open("Member Info.dat", ios::in | ios::binary);
	if (!inMemberFile) {
		cout << "File could not be opened" << '\n';
	}
	while (!inMemberFile.eof() && inMemberFile.read(reinterpret_cast<char*>(&memberDetails[numMembers]), sizeof(memberDetails[numMembers]))) {
		numMembers++;
	}
}
void loadBookingHistories(BookingInfo bookingHistories[], int& numBookings) {
	ifstream inBookingFile;
	inBookingFile.open("Booking Histories.dat", ios::in | ios::binary);
	if (!inBookingFile) {
		cout << "File could not be opened" << '\n';
	}
	while (!inBookingFile.eof() && inBookingFile.read(reinterpret_cast<char*>(&bookingHistories[numBookings]), sizeof(bookingHistories[numBookings]))) {
		numBookings++;
	}
}
void loadMovies(Movie movies[], int& numMovies) {
	ifstream inMovierFile;
	inMovierFile.open("Movies.dat", ios::in | ios::binary);
	while (!inMovierFile.eof() && inMovierFile.read(reinterpret_cast<char*>(&movies[numMovies]), sizeof(movies[numMovies]))) {
		numMovies++;
	}

}
void loadMovieNames(char movieNames[][60], int numMovies) {
	ifstream inClientFile("Movie Names.txt", ios::in);
	if (!inClientFile) {
		cout << "File could not be opened" << '\n';
	}
	int i = 0;
	while (!inClientFile.eof()) {
		inClientFile.getline(movieNames[i], 100, '\n');
		i++;
	}

	/*for (int i = 0; i < numMovies; i++) {
		cout << '\n' << i << '.' << "Movie:" << movieNames[i] << '\n';
	}*/
}
void loadAvailableDates(char availableDates[][12], int& numDates) {
	numDates = 0;
	ifstream inDateFile("Available Dates.txt", ios::in);
	if (!inDateFile) {
		cout << "File could not be opened" << '\n';
	}
	while (!inDateFile.eof() && inDateFile.getline(availableDates[numDates], 100, '\n')) {

		numDates++;
	}

}

int inputAnInteger(int begin, int end) {
	string n;
	getline(cin, n);
	if (n.size() > 2 || n.size() == 0)
		return -1;
	else if (n.size() == 1) {
		char a[1];
		a[0] = n[0];
		if (a[0] < begin + 48 || a[0] > end + 48) {
			return -1;
		}
		return a[0] - 48;
	}
	else {
		char a[2];
		a[0] = n[0];
		a[1] = n[1];

		if (a[0] < 49 || a[0] > end / 10 + 49 || a[1] < begin + 48 || a[1] > 48 + end % 10)
			return -1;
		else
			return ((a[0] - 48) * 10 + a[1] - 48);
	}


}
void signIn(MemberRecord memberDetails[], int numMembers, Movie movies[], char movieNames[][60], int numMovies,
	BookingInfo bookingHistories[], int& numBookings, char availableDates[][12], int numDates) {

	char Email_test[20];
	char password_test[24];
	int p = 0;

	cout << "Enter email address:";
	cin >> Email_test;
	cout << "Enter password:";
	cin >> password_test;

	cin.ignore();
	if (illegal(Email_test, password_test, memberDetails, numMembers,p)) {
		int r = 1;
		while (r) {
			cout << "\nEnter your choice:" << endl;
			cout << "1. Account Information\n";
			cout << "2. Buy Tickets\n";
			cout << "3. My Bookings\n";
			cout << "4. Sign Out\n\n";
			int choice;
			do {
				cout << "Enter your choice (1,4): ";
				choice=inputAnInteger(1, 4);
				cout << '\n';
			} while (choice == -1);

			switch (choice)
			{
			case 1:
				accountInfor(memberDetails, numMembers, p);
				break;

			case 2:
				displaySessionTimes(movies, movieNames, numMovies, availableDates, numDates);
				buyTickets(bookingHistories, numBookings, movies, movieNames, numMovies, availableDates, numDates, memberDetails[p].email);
				break;
			case 3:
				displayBookingHistory(bookingHistories, numBookings, movies, movieNames, availableDates, memberDetails[p].email);
				break;
			case 4:
				r--;
				break;


			default:
				cout << "Input Error!\n\n";
				break;
			}
		}
	}
	else {
		signIn(memberDetails, numMembers, movies, movieNames, numMovies,
			bookingHistories, numBookings, availableDates, numDates);
	}

}
bool illegal(char email[], char password[], MemberRecord memberDetails[], int numMembers, int& recordNumber) {
	int op = 0;
	int save_i= recordNumber;
	for (int i = 0; i < numMembers; i++) {
		if (strcmp(email, memberDetails[i].email) == 0) {
			save_i = i;
			op++;
		}
	}
		
	if (strcmp(password, memberDetails[save_i].password) == 0) {
			op++;
	}
	
	if (op == 2) {
		recordNumber = save_i;
		return true;
	}
	else {
		cout << "Sorry, unrecognized email or password." << '\n';
		return false;
	}
}
void accountInfor(MemberRecord memberDetails[], int numMembers, int recordNumber) {
	cout << "1. Name:" << memberDetails[recordNumber].name << '\n';
	cout << "2. Email Address:" << memberDetails[recordNumber].email << '\n';
	cout << "3. Phone Number:" << memberDetails[recordNumber].phone << '\n';
	cout << "4. ID Number:" << memberDetails[recordNumber].IDNumber << '\n';
	cout << "5. Password:" << memberDetails[recordNumber].password << '\n' << '\n';
	cout << "Which one do you want to modify (0 – not modify)?";
	int u = 1;
	while (u) {
		char eamil[20];
		char IDNumber[20];
		int num;
		num = inputAnInteger(0, 5);
		cout << endl;
		switch (num)
		{
		case 0:
			u--;
			break;
		case 1:
			cout << "Enter correct data :";
			memset(memberDetails[recordNumber].name, ' ', strlen(memberDetails[recordNumber].name));

			cin >> memberDetails[recordNumber].name;
			cout << '\n' << "Successful!\n\n";
			u--;
			break;
		case 2:
			cout << "Enter correct data :";
			memset(memberDetails[recordNumber].email, ' ', strlen(memberDetails[recordNumber].email));
			cin >> memberDetails[recordNumber].email;
			cout << '\n' << "Successful!\n\n";
			u--;
			break;

		case 3:
			cout << "Enter correct data :";
			memset(memberDetails[recordNumber].phone, ' ', strlen(memberDetails[recordNumber].phone));
			cin >> memberDetails[recordNumber].phone;
			cout << '\n' << "Successful!\n\n";
			u--;
			break;
		case 4:
			cout << "Enter correct data :";
			memset(memberDetails[recordNumber].IDNumber, ' ', strlen(memberDetails[recordNumber].IDNumber));
			cin >> memberDetails[recordNumber].IDNumber;
			cout << '\n' << "Successful!\n\n";
			u--;
			break;
		case 5:
			cout << "Enter correct data :";
			memset(memberDetails[recordNumber].password, ' ', strlen(memberDetails[recordNumber].password));
			cin >> memberDetails[recordNumber].password;
			cout << '\n' << "\nSuccessful!\n";
			u--;
			break;
		default:
			cout << "Input Error!Please try again:";
			break;
		}
	}


}
void buyTickets(BookingInfo bookingHistories[], int& numBookings, Movie movies[], char movieNames[][60],
	int numMovies, char availableDates[][12], int numDates, char email[]) {

	int movie, date, time, adult = 0, concession = 0, disability = 0, elder = 0;

	do {
		cout << "Enter movie code(0 - 11) : ";
		movie = inputAnInteger(0, 11);
		cout << '\n';
	} while (movie == -1);

	do {
		cout << "Enter date code (0 - 8): ";
		date = inputAnInteger(0, 8);
		cout << '\n';
	} while (date == -1 || movies[movie].dates[date] == 0);

	do {
		cout << "Enter session time code (0 - 16):";
		time = inputAnInteger(0, 11);
		cout << '\n';
	} while (date == -1 || movies[movie].sessionTimes[time] == 0);

	bookingHistories[numBookings].movieCode = movie;
	bookingHistories[numBookings].dateCode = date;
	bookingHistories[numBookings].sessionTimeCode = time;
	for (int i = 0; email[i] != '\0'; i++)
		bookingHistories[numBookings].email[i] = email[i];


	cout << "\nMovie: " << movieNames[movie];
	cout << "\nDate: " << availableDates[date];
	cout << "\nShow Time: " << hours[time];
	cout << "\nPrice: ";
	for (int i = 0; i < 4; i++) {
		switch (i)
		{
		case 0:
			cout << "Adult-";
			break;
		case 1:
			cout << "Concession-";
			break;
		case 2:
			cout << "Disability-";
			break;
		case 3:
			cout << "Elderly-";
			break;
		default:
			break;
		}
		cout << movies[movie].prices[i] << ", ";
	}
	while (adult == 0 && concession == 0 && disability == 0 && elder == 0) {
		do {
			cout << "\nEnter the number of adult tickets(0 - 6) :";
			adult = inputAnInteger(0, 6);
		} while (adult == -1);

		do {
			cout << "Enter the number of concession tickets(0 - 6) :";
			concession = inputAnInteger(0, 6);
		} while (concession == -1);

		do {
			cout << "Enter the number of disability tickets(0 - 6) :";
			disability = inputAnInteger(0, 6);
		} while (disability == -1);

		do {
			cout << "Enter the number of elderly tickets(0 - 6) :";
			elder = inputAnInteger(0, 6);
		} while (elder == -1);
	}
	bookingHistories[numBookings].numTickets[0] = adult;
	bookingHistories[numBookings].numTickets[1] = concession;
	bookingHistories[numBookings].numTickets[2] = disability;
	bookingHistories[numBookings].numTickets[3] = elder;
	display(movies, bookingHistories[numBookings]);
	selectSeats(bookingHistories, numBookings, movies);
	numBookings++;
}
void selectSeats(BookingInfo bookingHistories[], int numBookings, Movie movies[]) {
	/*bool occupiedSeats[9][17][8][12]; // occupiedSeats[i][j] is the occupied Seats for all accounts
										   // at j-th session time on i-th date
char seletedSeats[24][4]; // seleted seats for the user with the specified email*/
	cout << "  A B C D E F G H I J K L\n";
	for (int i = 0; i < 8; i++) {
		cout << i << ' ';
		for (int j = 0; j < 12; j++) {
			cout << movies[bookingHistories[numBookings].movieCode].occupiedSeats[bookingHistories[numBookings].dateCode][bookingHistories[numBookings].sessionTimeCode][i][j] << ' ';
		}
		cout << '\n';
	}
	int sumTickets = bookingHistories[numBookings].numTickets[0] + bookingHistories[numBookings].numTickets[1] + bookingHistories[numBookings].numTickets[2] + bookingHistories[numBookings].numTickets[3];

	cout << "Select " << sumTickets << "seats (e.g. 0A):\n";

	char seats[24][2];
	int number = 0;
	while (number < sumTickets) {
		cout << '?';
		cin >> seats[number];
		if (movies[bookingHistories[numBookings].movieCode].occupiedSeats[bookingHistories[numBookings].dateCode][bookingHistories[numBookings].sessionTimeCode][seats[number][0] - 48][seats[number][1] - 65] == 0) {

			movies[bookingHistories[numBookings].movieCode].occupiedSeats[bookingHistories[numBookings].dateCode][bookingHistories[numBookings].sessionTimeCode][seats[number][0] - 48][seats[number][1] - 65] = 1;
			bookingHistories[numBookings].seletedSeats[number][0] = seats[number][0] - 48;
			bookingHistories[numBookings].seletedSeats[number][1] = seats[number][1] - 65;
			number++;
		}

		else {
			cout << "This seat has been occupied. Please select another seat.\n";
		}
		
	}

	cout << "\nSuccessful!\n";

	cin.ignore();

}
void displaySessionTimes(Movie movies[], char movieNames[][60], int numMovies,
	char availableDates[][12], int numDates) {

	for (int i = 0; i < numMovies; i++) {
		cout << '\n' << i << '.' << "Movie:" << movieNames[i] << '\n' << "   Date:";
		for (int j = 0; j < numDates; j++) {
			if (movies[i].dates[j] == 1) {
				cout << "  " << j << '.' << availableDates[j] << ',';
			}
		}
		cout << "\n   Session Time:";

		for (int j = 0; j < 17; j++) {
			if (movies[i].sessionTimes[j] == 1) {
				cout << "  " << j << ". " << hours[j] << '.';
			}
		}
		cout << '\n';
	}

}
void display(Movie movies[], BookingInfo bookingHistory) {
	cout << " \n\n           No. of Tickets  Price  Subtotal\n";
	for (int i = 0; i < 4; i++) {
		if (bookingHistory.numTickets[i] != 0) {
			switch (i)
			{
			case 0:
				cout << "Adult" << setw(20) << bookingHistory.numTickets[0] << "   " << movies[bookingHistory.movieCode].prices[0] << "      " << bookingHistory.numTickets[0] * movies[bookingHistory.movieCode].prices[0] << '\n';
				break;
			case 1:
				cout << "Concession" << setw(15) << bookingHistory.numTickets[1] << "   " << movies[bookingHistory.movieCode].prices[1] << "      " << bookingHistory.numTickets[1] * movies[bookingHistory.movieCode].prices[1] << '\n';
				break;
			case 2:
				cout << "Disability" << setw(15) << bookingHistory.numTickets[2] << "   " << movies[bookingHistory.movieCode].prices[2] << "      " << bookingHistory.numTickets[2] * movies[bookingHistory.movieCode].prices[2] << '\n';
				break;
			case 3:
				cout << "Elderly" << setw(18) << bookingHistory.numTickets[3] << "   " << movies[bookingHistory.movieCode].prices[3] << "      " << bookingHistory.numTickets[3] * movies[bookingHistory.movieCode].prices[3] << '\n';
				break;
			default:
				break;
			}
		}
	}
	cout << "\nTotal Amount For Tickets:" << bookingHistory.numTickets[0] * movies[bookingHistory.movieCode].prices[0] + bookingHistory.numTickets[1] * movies[bookingHistory.movieCode].prices[1] +
		bookingHistory.numTickets[2] * movies[bookingHistory.movieCode].prices[2] + bookingHistory.numTickets[3] * movies[bookingHistory.movieCode].prices[3] << '\n';
}
void displayBookingHistory(BookingInfo bookingHistories[], int numBookings, Movie movies[],
	char movieNames[][60], char availableDates[][12], char email[]) {
	int op = 0;
	int y;
	for (int i = 0; i < numBookings; i++)
		if (strcmp(bookingHistories[i].email, email) == 0) {
			op++;
			y = i;
		}
	

	if (op == 1) {
		int sumTickets = bookingHistories[y].numTickets[0] + bookingHistories[y].numTickets[1] + bookingHistories[y].numTickets[2] + bookingHistories[y].numTickets[3];
		cout << "\nBooking History: \n";
		cout << "\nMovie: " << movieNames[bookingHistories[y].movieCode];
		cout << "\nDate: " << availableDates[bookingHistories[y].dateCode];
		cout << "\nShow Time: " << hours[bookingHistories[y].sessionTimeCode];
		cout << "\nSeats: ";
		for (int i = 0; i < sumTickets; i++) {
			char save[2];
			save[0] = bookingHistories[y].seletedSeats[i][1] + 65;
			save[1] = bookingHistories[y].seletedSeats[i][0] + 48;
			cout << ' ' << save[1] << save[0];
		}
		display(movies,bookingHistories[y]);
	}
	else
		cout << "No bookings!\n";
}

void newMember(MemberRecord memberDetails[], int& numMembers) {
	char IDNumber_test[20];
	char email_test[20];
	char name_test[20];
	int i = 1;
	cout << "Enter your ID number:";
	cin >> IDNumber_test;
	if (!existingID(IDNumber_test, memberDetails, numMembers)) {
		for (int i = 0; IDNumber_test[i] != '\0'; i++)
			memberDetails[numMembers].IDNumber[i] = IDNumber_test[i];
		cout << "Enter your name :";
		cin >> name_test;

		while (1) {
			cout << "Enter an email address:";
			cin >> email_test;
			if (!existingEmail(email_test, memberDetails, numMembers)) {
				for (int i = 0; email_test[i] != '\0'; i++)
					memberDetails[numMembers].email[i] = email_test[i];
				cout << "Enter a password:";
				cin >> memberDetails[numMembers].password;
				cout << "Enter your phone number:";
				cin >> memberDetails[numMembers].phone;

				cout << "Successful!" << endl << endl;

				for (int i = 0; name_test[i] != '\0'; i++)
					memberDetails[numMembers].name[i] = name_test[i];

				numMembers++;
				return;
			}
			else {
				cout << "\nAn account already exists with the email address!\n\n";
				
			}
		}
	}
	else {
		cout << "\nAn account already exists with the ID number!\n\n";
		return;
	}
}
bool existingID(char newIDNumber[], MemberRecord memberDetails[], int& numMembers) {
	for (int i = 0; i < numMembers; i++) {
		if (strcmp(newIDNumber, memberDetails[i].IDNumber) == 0)
			return true;
	}
	return false;
}
bool existingEmail(char newEmail[], MemberRecord memberDetails[], int& numMembers) {//存在的時候回傳true
	for (int i = 0; i < numMembers; i++) {
		if (strcmp(newEmail, memberDetails[i].email) == 0) {
			return true;
		}

	}
	return false;
}

void saveMemberInfo(MemberRecord memberDetails[], int numMembers) {
	ofstream outMenberFile;
	outMenberFile.open("Member Info.dat", ios::out | ios::binary);

	for (int i = 0; i < numMembers; i++)
		outMenberFile.write(reinterpret_cast<const char*>(&memberDetails[i]), sizeof(memberDetails[0]));
}
void saveMovies(Movie movies[], int numMovies) {
	ofstream outMenberFile;
	outMenberFile.open("Movies.dat", ios::out | ios::binary);

	for (int i = 0; i < numMovies; i++)
		outMenberFile.write(reinterpret_cast<const char*>(&movies[i]), sizeof(movies[0]));
}
void saveBookingHistories(BookingInfo bookingHistories[], int numBookings) {
	ofstream outMenberFile;
	outMenberFile.open("Booking Histories.dat", ios::out | ios::binary);
	for (int i = 0; i < numBookings; i++)
		outMenberFile.write(reinterpret_cast<const char*>(&bookingHistories[i]), sizeof(bookingHistories[0]));
}