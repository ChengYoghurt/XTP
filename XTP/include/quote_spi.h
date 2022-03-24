#pragma once
#include "xtp_quote_api.h"
#include <fstream>
#include <time.h>
#include <sys/timeb.h>
#include <vector>
#include <ctime>
#include <map>
#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif // _WIN32

#define NUM_OF_ROUND 1

using namespace XTP::API;

typedef struct XTPDepthMarketEveryTicker{
	std::vector<uint32_t>	 vec_depthtime;
	std::vector<uint32_t>	 vec_localtime;
	std::vector<int32_t>     vec_status;
	std::vector<double> 	 vec_overall_price[5];
	std::vector<double>	     vec_askprice[10];
	std::vector<int32_t>     vec_askvolume[10];
	std::vector<double>      vec_bidprice[10];
	std::vector<int32_t>     vec_bidvolume[10];
	std::vector<double>      vec_trades;
	std::vector<int32_t>     vec_volume;
	std::vector<double>      turnover;
	std::vector<int32_t>      vec_totalbidvol;
	std::vector<int32_t>     vec_totalaskvol;
    std::vector<double>      vec_WeightedAvgBidPrice;
	std::vector<double>      vec_WeightedAvgAskPrice;
	std::vector<double>      vec_HighLimit;
	std::vector<double>      vec_LowLimit;
}XTPDMET;


class MyQuoteSpi : public QuoteSpi
{
public:
	MyQuoteSpi();
	~MyQuoteSpi();

	///���ͻ����뽻�׺�̨ͨ�����ӶϿ�ʱ���÷��������á���������������API���Զ��������ӣ��ͻ��˿ɲ���������
	///@param reason ����ԭ��
	///        0x1001 �����ʧ��
	///        0x1002 ����дʧ��
	///        0x2001 ����������ʱ
	///        0x2002 ��������ʧ��
	///        0x2003 �յ�������
	virtual void OnDisconnected(int reason);

///����Ӧ��
	virtual void OnError(XTPRI *error_info,bool is_last);


	virtual void OnSubMarketData(XTPST *ticker, XTPRI *error_info, bool is_last);
	virtual void OnUnSubMarketData(XTPST *ticker, XTPRI *error_info, bool is_last);
	virtual void OnDepthMarketData(XTPMD *market_data, int32_t bid1_qty[], int32_t bid1_count, int32_t max_bid1_count, int32_t ask1_qty[], int32_t ask1_count, int32_t max_ask1_count);
	virtual void OnSubOrderBook(XTPST *ticker, XTPRI *error_info, bool is_last);
	virtual void OnUnSubOrderBook(XTPST *ticker, XTPRI *error_info, bool is_last);
	virtual void OnSubTickByTick(XTPST *ticker, XTPRI *error_info, bool is_last);
	virtual void OnUnSubTickByTick(XTPST *ticker, XTPRI *error_info, bool is_last);
	virtual void OnOrderBook(XTPOB *order_book);
	virtual void OnTickByTick(XTPTBT *tbt_data);
	virtual void OnQueryAllTickers(XTPQSI* ticker_info, XTPRI *error_info, bool is_last);
	virtual void OnQueryTickersPriceInfo(XTPTPI* ticker_info, XTPRI *error_info, bool is_last);
	virtual void OnSubscribeAllMarketData(XTP_EXCHANGE_TYPE exchange_id, XTPRI *error_info);
	virtual void OnUnSubscribeAllMarketData(XTP_EXCHANGE_TYPE exchange_id, XTPRI *error_info);
	virtual void OnSubscribeAllOrderBook(XTP_EXCHANGE_TYPE exchange_id, XTPRI *error_info);
	virtual void OnUnSubscribeAllOrderBook(XTP_EXCHANGE_TYPE exchange_id, XTPRI *error_info);
	virtual void OnSubscribeAllTickByTick(XTP_EXCHANGE_TYPE exchange_id, XTPRI *error_info);
	virtual void OnUnSubscribeAllTickByTick(XTP_EXCHANGE_TYPE exchange_id, XTPRI *error_info);
	virtual void OnSubscribeAllOptionMarketData(XTP_EXCHANGE_TYPE exchange_id, XTPRI *error_info);
	virtual void OnUnSubscribeAllOptionMarketData(XTP_EXCHANGE_TYPE exchange_id, XTPRI *error_info);
	virtual void OnSubscribeAllOptionOrderBook(XTP_EXCHANGE_TYPE exchange_id, XTPRI *error_info);
	virtual void OnUnSubscribeAllOptionOrderBook(XTP_EXCHANGE_TYPE exchange_id, XTPRI *error_info);
	virtual void OnSubscribeAllOptionTickByTick(XTP_EXCHANGE_TYPE exchange_id, XTPRI *error_info);
	virtual void OnUnSubscribeAllOptionTickByTick(XTP_EXCHANGE_TYPE exchange_id, XTPRI *error_info);
	virtual void print_vec_xtpdmet(const std::map<std::string, XTPDMET> map_xtpdmet, const std::string file_path) const;
	virtual void print_ticker_info(const int& print_type, const char* query_ticker_path) const;
	const std::map<std::string, XTPDMET>&get_xtpdmet(){
		return map_xtpdmet;
	}
private:
	std::vector <XTPQSI> ticker_sh;
	std::vector <XTPQSI> ticker_sz;
	std::map<std::string, XTPDMET> map_xtpdmet;

	bool IsErrorRspInfo(XTPRI *pRspInfo);	
};