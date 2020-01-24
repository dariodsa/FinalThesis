enum QUERY_TYPE{
    CONST
    , RANGE
    , FULL_INDEX_SCAN
    , FULL_TABLE_SCAN
};

class Token {
    public:
        Token(QUERY_TYPE token, int number_of_rows) {
            this->token = token;
            this->number_of_rows = number_of_rows;
        }
        QUERY_TYPE getToken() {
            return this->token;
        }

    private:
        QUERY_TYPE token;
        int number_of_rows;
};