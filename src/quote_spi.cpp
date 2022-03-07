#include "quote_spi.h"
#include <iostream>
#include <stdio.h>
#include <fstream>
using namespace std;



void MyQuoteSpi::OnError(XTPRI *error_info, bool is_last)
{
	cout << "--->>> "<< "OnRspError" << endl;
	IsErrorRspInfo(error_info);
}

MyQuoteSpi::MyQuoteSpi()
{
}

MyQuoteSpi::~MyQuoteSpi()
{
}

void MyQuoteSpi::OnDisconnected(int reason)
{
	cout << "--->>> " << "OnDisconnected quote" << endl;
	cout << "--->>> Reason = " << reason << endl;
	//���ߺ󣬿�����������
	//�������ӳɹ�����Ҫ���������������������?
}

void MyQuoteSpi::OnSubMarketData(XTPST *ticker, XTPRI *error_info, bool is_last)
{
	cout << "OnRspSubMarketData -----" << endl;

}

void MyQuoteSpi::OnUnSubMarketData(XTPST *ticker, XTPRI *error_info, bool is_last)
{
 	cout << "OnRspUnSubMarketData -----------" << endl;
}

void MyQuoteSpi::OnDepthMarketData(XTPMD * market_data, int64_t bid1_qty[], int32_t bid1_count, int32_t max_bid1_count, int64_t ask1_qty[], int32_t ask1_count, int32_t max_ask1_count)
{
	
	vector_XTPMD.push_back(*market_data);
	
}

void MyQuoteSpi::OnSubOrderBook(XTPST *ticker, XTPRI *error_info, bool is_last)
{

}

void MyQuoteSpi::OnUnSubOrderBook(XTPST *ticker, XTPRI *error_info, bool is_last)
{

}

void MyQuoteSpi::OnSubTickByTick(XTPST *ticker, XTPRI *error_info, bool is_last)
{

}

void MyQuoteSpi::OnUnSubTickByTick(XTPST * ticker, XTPRI * error_info, bool is_last)
{
}

void MyQuoteSpi::OnOrderBook(XTPOB *order_book)
{

}

void MyQuoteSpi::OnTickByTick(XTPTBT *tbt_data)
{
	//cout<<tbt_data->data_time<<endl;
}

void MyQuoteSpi::OnQueryAllTickers(XTPQSI * ticker_info, XTPRI * error_info, bool is_last)
{
	cout << "OnQueryAllTickers -----------" << endl;
}

void MyQuoteSpi::OnQueryTickersPriceInfo(XTPTPI * ticker_info, XTPRI * error_info, bool is_last)
{
}

void MyQuoteSpi::OnSubscribeAllMarketData(XTP_EXCHANGE_TYPE exchange_id, XTPRI * error_info)
{
}

void MyQuoteSpi::OnUnSubscribeAllMarketData(XTP_EXCHANGE_TYPE exchange_id, XTPRI * error_info)
{
}

void MyQuoteSpi::OnSubscribeAllOrderBook(XTP_EXCHANGE_TYPE exchange_id, XTPRI * error_info)
{
}

void MyQuoteSpi::OnUnSubscribeAllOrderBook(XTP_EXCHANGE_TYPE exchange_id, XTPRI * error_info)
{
}

void MyQuoteSpi::OnSubscribeAllTickByTick(XTP_EXCHANGE_TYPE exchange_id, XTPRI * error_info)
{
}

void MyQuoteSpi::OnUnSubscribeAllTickByTick(XTP_EXCHANGE_TYPE exchange_id, XTPRI * error_info)
{
}

void MyQuoteSpi::OnSubscribeAllOptionMarketData(XTP_EXCHANGE_TYPE exchange_id, XTPRI * error_info)
{
}

void MyQuoteSpi::OnUnSubscribeAllOptionMarketData(XTP_EXCHANGE_TYPE exchange_id, XTPRI * error_info)
{
}

void MyQuoteSpi::OnSubscribeAllOptionOrderBook(XTP_EXCHANGE_TYPE exchange_id, XTPRI * error_info)
{
}

void MyQuoteSpi::OnUnSubscribeAllOptionOrderBook(XTP_EXCHANGE_TYPE exchange_id, XTPRI * error_info)
{
}

void MyQuoteSpi::OnSubscribeAllOptionTickByTick(XTP_EXCHANGE_TYPE exchange_id, XTPRI * error_info)
{
}

void MyQuoteSpi::OnUnSubscribeAllOptionTickByTick(XTP_EXCHANGE_TYPE exchange_id, XTPRI * error_info)
{
}

bool MyQuoteSpi::IsErrorRspInfo(XTPRI *pRspInfo)
{
	// ���ErrorID != 0, ˵���յ��˴�������?
	bool bResult = ((pRspInfo) && (pRspInfo->error_id != 0));
	if (bResult)
		cout << "--->>> ErrorID=" << pRspInfo->error_id << ", ErrorMsg=" << pRspInfo->error_msg << endl;
	return bResult;
}

void MyQuoteSpi::print_vec_xtpmd(std::vector<XTPMD> &vec_xtpmd, char* file_path){

	char market_data_path[100] = {'\0'};
	sprintf(market_data_path, "%s_market_data.csv", file_path);
    std::ofstream market_data_outfile;
	market_data_outfile.open(market_data_path, std::ios::out); 
	
	market_data_outfile << "data_time" << ","
	<< "ticker" << ","
	<< "last_price" << ","
	<< "qty" << ","
	<< "turnover" << ","
	<< "bid" << ","
	<< "bit_qty" << ","
	<< "ask" << ","
	<< "ask_qty" << ","
	<< "trades_count" << ","
	<< "local_time" << std::endl;

    constexpr std::size_t k_max_depth_level = 10;
	size_t index;
	time_t local_time = time(NULL);
	tm *tm_local_time = localtime(&local_time);
    for(auto& vec_xtpmd_item : vec_xtpmd){
        
        XTPMD market_data = vec_xtpmd_item;
        market_data_outfile << market_data.data_time << "," 
		<< market_data.ticker << ","
        << market_data.last_price << ","
        << market_data.qty << "," 
        << market_data.turnover << ","; 
        
        for(index = 0; index < k_max_depth_level; index++){
		    market_data_outfile << market_data.bid[index] << ' ';
	    }
	    market_data_outfile << ",";

	    for(index = 0; index < k_max_depth_level; index++){
		    market_data_outfile<<market_data.bid_qty[index] << ' ';
	    }
	    market_data_outfile<<",";

	    for(index = 0; index < k_max_depth_level; index++){
		    market_data_outfile<<market_data.ask[index] << ' ';
	    }
	    market_data_outfile<<",";

	    for(index = 0; index < k_max_depth_level; index++){
		    market_data_outfile<<market_data.ask_qty[index] << ' ';
	    }
	    market_data_outfile << ",";
	    market_data_outfile << market_data.trades_count << "," 
		<< asctime(tm_local_time) << std::endl;
    }

	market_data_outfile.close();
}

