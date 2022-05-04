/* ----------------------------------------------------
* DBS211 SECTION - NEE
* DATABASE APPLICATION DEVELOPMENT ASSIGNMENT
* MILESTONE 1 AND 2 COMBINED (SINGLE SUBMISSION)

/*system libraries */

#include <iostream>
#include <occi.h>

/* user libraries */
#include <iomanip>
#include <limits>
#include <string>

/* struct defintion */
struct Employee
{
    int employeeNumber;
    char lastName[50];
    char firstName[50];
    char email[100];
    char phone[20];
    char reportsTo[100];
    char job_id[50]; // added
    char manager_id[100]; //added
    char jobTitle[100];
    char city[50];
};

using namespace oracle::occi;
using namespace std;

/* function prototypes */

/* 1st milestone */
int menu();
int findEmployee(Connection* conn, int employeeNumber, struct Employee* emp);
void displayEmployee(Connection* conn, struct Employee* emp);
void displayAllEmployees(Connection* conn);

/* 2nd Milestone */
void insertEmployee(struct Employee* emp);
void insertEmployee(Connection* conn, struct Employee emp);
void getNewInfo(const char* _prompt, int& src);
void getNewInfo(const char* _prompt, char* src);
void updateEmployee(Connection* conn, int employeeNumber);
int getEmployeeNumber();
void deleteEmployee(Connection* conn, int employeeNumber);


/* main begins execution */
int main()
{
    /* creating connection to database */
    std::string usr = "";
    std::string pass = "";
    std::string srv = "myoracle12c.senecacollege.ca:1521/oracle12c";

    Environment* env = Environment::createEnvironment(Environment::DEFAULT);
    Connection* conn = env->createConnection(usr, pass, srv);

    Employee* emp = new Employee;
    struct Employee employee = {};
    int choice = -1;
    while (choice != 0)
    {
        choice = menu();
        switch (choice)
        {
        case 0:
            std::cout << "Exiting" << std::endl;
            exit(0);

        case 1:
            int num;
            int found;
            std::cout << "Enter Employee Number: ";
            while (!(std::cin >> num))
            {
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                std::cout << "Invalid input.  Try again: ";
            }
            found = findEmployee(conn, num, emp);
            if (found == 1)
            {
                displayEmployee(conn, emp);
            }
            if (found == 0)
            {
                std::cout << "Employee " << num << " does not exist." << std::endl;
            }
            break;

        case 2:
            displayAllEmployees(conn);
            break;

        case 3:
            insertEmployee(&employee);
            if (findEmployee(conn, employee.employeeNumber, &employee) != 1)
            {
                insertEmployee(conn, employee);
            }
            else
            {
                cout << endl;
                cout << "An employee with the same employee number exists.";
                cout << endl << endl << endl;
            }
            break;

        case 4:
        {
            int employeeNumber = getEmployeeNumber();
            updateEmployee(conn, employeeNumber);
        }

        system("pause");
        break;

        case 5:
        {
            int employeeNumber = getEmployeeNumber();
            deleteEmployee(conn, employeeNumber);
            break;
        }

        default:
        {
            std::cout << "You need to enter a number between 0 and 5" << std::endl;
            break;

        }
        }
    }
}

/* cascading menu system */
int menu()
{
    int opt = -1;
    std::cout << "****************************************HR MENU************************************************" << std::endl;
    std::cout << "DYLAN RUKAVINA, JACKSON PEDVIS, KHUSHBOO JAYESH DAVEY, RUBALDEEP KAUR, RAVNEET KAUR, YUJIN BONG" << std::endl;
    std::cout << "***********************************************************************************************" << std::endl;
    std::cout << endl;
    std::cout << "1) Find Employee" << std::endl;
    std::cout << "2) Report of Employees" << std::endl;
    std::cout << "3) Add Employee" << std::endl;
    std::cout << "4) Update Employee" << std::endl;
    std::cout << "5) Remove Employee" << std::endl;
    std::cout << "0) Exit" << std::endl;
    std::cout << endl;
    std::cout << "Enter an option (0-5) : ";

    while (!(std::cin >> opt))
    {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "Invalid input. Try again: ";
    }return opt;
}

/* this function returns 0 if the employee does not exist else returns 1 if the employee exists */
int findEmployee(Connection* conn, int employeeNumber, struct Employee* employee)
{
    Statement* stmt = conn->createStatement();
    std::string chck = "SELECT COUNT(*) FROM employees WHERE employee_id = " + std::to_string(employeeNumber);
    ResultSet* ck = stmt->executeQuery(chck);

    int i = 0;
    while (ck->next())
    {
        i = ck->getInt(1);
        if (i < 1)
        {
            conn->terminateStatement(stmt); return 0;
        }
    }

    try
    {
        conn->terminateStatement(stmt);
        std::string qry = "select e.first_name as fname, e.last_name as lname, e.phone_number as phone, e.email as email, m.first_name || ' ' || m.last_name as manager, e.job_id as job_title, l.city as city from employees e left join employees m on e.manager_id = m.employee_id left join departments d on e.department_id = d.department_id left join locations l on d.location_id = l.location_id where e.employee_id = " + std::to_string(employeeNumber);

        Statement* qr = conn->createStatement();
        ResultSet* rs = qr->executeQuery(qry);

        while (rs->next())
        {
            int fs = sizeof(rs->getString(1)),
                ls = sizeof(rs->getString(2)),
                ps = sizeof(rs->getString(3)),
                es = sizeof(rs->getString(4)),
                ms = sizeof(rs->getString(5)),
                js = sizeof(rs->getString(6)),
                cs = sizeof(rs->getString(7));

            employee->employeeNumber = employeeNumber;

            for (int m = 0; m < fs; m++)
            {
                employee->firstName[m] = rs->getString(1)[m];
            }

            for (int j = 0; j < ls; j++)
            {
                employee->lastName[j] = rs->getString(2)[j];
            }

            for (int l = 0; l < ps; l++)
            {
                employee->phone[l] = rs->getString(3)[l];
            }

            for (int k = 0; k < es; k++)
            {
                employee->email[k] = rs->getString(4)[k];
            }

            for (int g = 0; g < ms; g++)
            {
                employee->reportsTo[g] = rs->getString(5)[g];
            }

            for (int h = 0; h < js; h++)
            {
                employee->jobTitle[h] = rs->getString(6)[h];
            }

            for (int a = 0; a < cs; a++)
            {
                employee->city[a] = rs->getString(7)[a];
            }
        }conn->terminateStatement(qr);
        return 1;
    }
    catch (SQLException& sqlExcpt)
    {
        std::cout << sqlExcpt.getMessage();
    }
}

/* this function displays employee's information and if the given employee number doesn't exist then the function displays error message */
void displayEmployee(Connection* conn, struct Employee* emp)
{
    int id = emp->employeeNumber;
    char* fn = { &emp->firstName[0] };
    char* ln = { &emp->lastName[0] };
    char* em = { &emp->email[0] };
    char* ph = { &emp->phone[0] };
    char* rt = { &emp->reportsTo[0] };
    char* jt = { &emp->jobTitle[0] };
    char* ci = { &emp->city[0] };

    printf("ID               : %i\nFirst Name       : %s\nLast Name        : %s\nEmail            : %s@company.ca\nPhone Number     : %s\nReports To       : %s\nJob ID           : %s\nCity             : %s\n", id, fn, ln, em, ph, rt, jt, ci);
}

/* this function displays the following attributes for all employees  */
void displayAllEmployees(Connection* conn)
{
    Statement* stmt = conn->createStatement();
    ResultSet* rs = stmt->executeQuery("select e.employee_id as enumber, e.first_name as fname, e.last_name as lname, e.phone_number as phone, e.email as email, m.first_name || ' ' || m.last_name as manager, e.job_id as job_title, l.city as city from employees e left join employees m on e.manager_id = m.employee_id left join departments d on e.department_id = d.department_id left join locations l on d.location_id = l.location_id");

    std::cout << std::left << "Employees:" << "\n" << std::endl;
    std::cout << std::left << std::setw(10) << "ID" << std::setw(25) << "Employee Name" << std::left << std::setw(34) << "Email" << std::left << std::setw(27) << "Phone" << std::left << std::setw(26) << "Manager Name" << std::endl;
    std::cout << "------------------------------------------------------------------------------------------------------------------" << std::endl;

    while (rs->next())
    {
        int id = rs->getInt(1);
        std::string fname = rs->getString(2);
        std::string lname = rs->getString(3);
        std::string phone = rs->getString(4);
        std::string email = rs->getString(5);
        std::string mName = rs->getString(6);
        std::string jobid = rs->getString(7);
        std::string city = rs->getString(8);

        std::cout <<
            std::left << std::setw(10) << id <<
            std::left << std::setw(25) << fname + " " + lname <<
            std::left << std::setw(34) << email + "@companyemail.ca" <<
            std::left << std::setw(27) << phone <<
            std::left << std::setw(26) << mName <<
            std::endl;
    }
    conn->terminateStatement(stmt);
    std::cout << "\n" << std::endl;
}

/* this function gets new employees information */
void insertEmployee(struct Employee* emp)
{
    std::cout << "-------------- New Employee Information --------------" << endl;
    getNewInfo("Employee Number : ", emp->employeeNumber);
    getNewInfo("Last Name       : ", emp->lastName);
    getNewInfo("First Name      : ", emp->firstName);
    getNewInfo("Phone Number    : ", emp->phone);
    getNewInfo("Email           : ", emp->email);
    getNewInfo("Manager ID      : ", emp->manager_id);
    getNewInfo("Job Title       : ", emp->jobTitle);
}

void getNewInfo(const char* _prompt, int& src)
{
    bool _escapeFlag = false;
    cout << _prompt;
    do
    {
        cin >> src;

        if (!cin)
        {
            cout << "Only enter the integer, try again: ";
            cin.clear();
            cin.ignore(32767, '\n');
        }
        else
        {
            _escapeFlag = true;
            cin.ignore(32767, '\n');
        }
    } while (!_escapeFlag);
}

void getNewInfo(const char* _prompt, char* src)
{
    bool _escapeFlag = false;
    cout << _prompt;
    do {
        cin.getline(src, 100);
        if (!cin) {
            cout << "Exceeded the number of characters, try again: ";
            cin.clear();
            cin.ignore(32767, '\n');
        }
        else _escapeFlag = true;
    } while (!_escapeFlag);
}

/* this function inserts employees information into the database */
void insertEmployee(Connection* conn, struct Employee emp)
{
    try {
        Statement* stmt = conn->createStatement();

        /* execute a querty and store a result into a result set object */
        stmt->setSQL("INSERT INTO employees(employee_id, last_name, first_name, phone_number, email, manager_id, job_id, hire_date)  VALUES (:1, :2, :3, :4, :5, :6, :7 , sysdate)");

        stmt->setInt(1, emp.employeeNumber);
        stmt->setString(2, emp.lastName);
        stmt->setString(3, emp.firstName);
        stmt->setString(4, emp.phone);
        stmt->setString(5, emp.email);
        stmt->setString(6, emp.manager_id);
        stmt->setString(7, emp.jobTitle);

        stmt->executeUpdate();

        cout << endl;
        cout << "The new employee is added successfully." << endl;

        conn->commit();
        conn->terminateStatement(stmt);
    }
 
    catch (SQLException& sqlExcp)
    {
        cout << sqlExcp.getErrorCode() << ": " << sqlExcp.getMessage();
    }
}

/* this function updates information to the existing employee */
void updateEmployee(Connection* conn, int employeeNumber)
{
    try
    {
        struct Employee emp = {};

        /* If employee number exists */
        if (findEmployee(conn, employeeNumber, &emp) == 1)
        {
            cout << "Last Name: " << emp.lastName << endl;
            cout << "First Name: " << emp.firstName << endl;
            getNewInfo("Phone: ", emp.phone);

            Statement* stmt = conn->createStatement("UPDATE employees SET phone_number  = :1 "
                "WHERE employee_id = :2");

            stmt->setString(1, emp.phone);
            stmt->setInt(2, employeeNumber);
            stmt->executeQuery();

            cout << endl;
            cout << "The employee's phone number is updated successfully." << endl << endl;

            conn->commit();
            conn->terminateStatement(stmt);
        }
        else
        {
            /* If employee number doesn't exist */
            cout << endl;
            cout << "The employee with ID " << employeeNumber << " does not exist." << endl << endl;
        }
    }
    /* error? */
    catch (SQLException& sqlExcp)
    {
        cout << sqlExcp.getErrorCode() << ": " << sqlExcp.getMessage();
    }
}

/* this function validates whether the employee exists or not */
int getEmployeeNumber()
{
    int employeeNumber;
    cout << endl;
    cout << "Enter Employee Number : ";
    cin >> employeeNumber;

    while (cin.fail())
    {
        cout << endl;
        cout << "Invalid employee number try again please : ";
        cin.clear();
        cin.ignore(5000, '\n');
        cin >> employeeNumber;
    };
    cin.ignore();
    return employeeNumber;
}

/* this function deletes employees information into the database */
void deleteEmployee(Connection* conn, int employeeNumber)
{
    try
    {
        struct Employee emp = {};

        /* If employee number exists */
        if (findEmployee(conn, employeeNumber, &emp) == 1)
        {
            Statement* stmt = conn->createStatement("DELETE FROM EMPLOYEES WHERE employee_id = :1 ");
            stmt->setInt(1, employeeNumber);
            stmt->executeQuery();

            cout << endl;
            cout << "The employee has been removed successfully." << endl << endl;

            conn->commit();
            conn->terminateStatement(stmt);
        }
        else
        {
            /* If employee number does not exist */
            cout << endl;
            cout << "The employee with ID " << employeeNumber << " does not exist.";
            cout << endl << endl << endl;
        }
    }
    /* error? */
    catch (SQLException& sqlExcp)
    {
        cout << sqlExcp.getErrorCode() << ": " << sqlExcp.getMessage();
    }
}
