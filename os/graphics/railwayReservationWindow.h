#pragma once

#include "windows.h"
#include "../system/process.h"
#include "../system/processManager.h"
#include <string>
#include <vector>
using namespace std;

class RailwayReservationWindow : public Window {
private:
    struct TicketRecord {
        int id;
        string passenger;
        string train;
        string source;
        string destination;
        string travelDate;
        string status;
    };

    enum class Field {
        Passenger,
        Train,
        Source,
        Destination,
        TravelDate,
        CancelId,
        None
    };

    ProcessManager* processManager;
    process_id_t pid;
    vector<TicketRecord> tickets;
    string bookingFilePath;
    string passengerInput;
    string trainInput;
    string sourceInput;
    string destinationInput;
    string dateInput;
    string cancelInput;
    string statusMessage;
    Field activeField;
    int listScroll;
    int nextTicketId;

    void loadTickets();
    void saveTickets() const;
    void appendStatus(const string& message);
    process_id_t startRequestProcess(const string& requestName, int memoryUsage);
    void finishRequestProcess(process_id_t requestPid);
    void handleTextInput(string& value, int maxLength);
    void bookTicket();
    void cancelTicket();
    TicketRecord* findTicketById(int ticketId);
    const TicketRecord* findTicketById(int ticketId) const;
    static vector<string> split(const string& text, char delimiter);
    static string trim(const string& text);
    static bool isDigits(const string& text);

public:
    RailwayReservationWindow(ProcessManager* pm, process_id_t processId);

    void update() override;
    void drawContent() override;

    process_id_t getPid() const;
};
