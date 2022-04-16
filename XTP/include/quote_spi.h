#pragma once
#include "xtp_quote_api.h"
#include <fstream>
#include <time.h>
#include <sys/timeb.h>
#include <vector>
#include <ctime>
#include <map>
#include <thread>
#include <mutex>
#include <condition_variable>
#include "QuoteTypeDefs.h"

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif // _WIN32

#define NUM_OF_ROUND 1

using namespace XTP::API;
using instrument_id_t = std::string;

struct XTPDepthMarketEveryTicker{
	std::vector<uint32_t>	         vec_depthtime;
	std::vector<l2agg::timestamp_t>	 vec_localtime;
	std::vector<std::string> 		 vec_status;
	std::vector<l2agg::price_t> 	 vec_overall_price[5];
	std::vector<l2agg::price_t>	     vec_askprice[10];
	std::vector<l2agg::volume_t>     vec_askvolume[10];
	std::vector<l2agg::price_t>      vec_bidprice[10];
	std::vector<l2agg::volume_t>     vec_bidvolume[10];
	std::vector<l2agg::price_t>      vec_trades;
	std::vector<l2agg::volume_t>     vec_volume;
	std::vector<l2agg::price_t>      vec_turnover;
	std::vector<l2agg::volume_t>     vec_totalbidvol;
	std::vector<l2agg::volume_t>     vec_totalaskvol;
    std::vector<l2agg::price_t>      vec_WeightedAvgBidPrice;
	std::vector<l2agg::price_t>      vec_WeightedAvgAskPrice;
	std::vector<l2agg::price_t>      vec_HighLimit;
	std::vector<l2agg::price_t>      vec_LowLimit;
};

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
	virtual void OnDepthMarketData(XTPMD *market_data, int64_t bid1_qty[], int32_t bid1_count, int32_t max_bid1_count, int64_t ask1_qty[], int32_t ask1_count, int32_t max_ask1_count);
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
	virtual void print_vec_xtpdmet(const std::string &file_path) const;
	virtual void print_ticker_info(XTP_EXCHANGE_TYPE exchange_id, const std::string query_ticker_path) const;
	const std::map<std::string, XTPDepthMarketEveryTicker>&get_xtpdmet(){
		return map_xtpdmet_;
	}
	std::condition_variable cv_last;
	bool processed_sh;
	bool processed_sz;

private:
	std::vector <std::string> ticker_sh;
	std::vector <std::string> ticker_sz;
	std::map<instrument_id_t, XTPDepthMarketEveryTicker> map_xtpdmet_;

	bool IsErrorRspInfo(XTPRI *pRspInfo);	
};