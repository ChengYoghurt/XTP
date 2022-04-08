#include "quote_spi.h"
#include "KuafuUtils.h"
#include "YAMLGetField.h"
#include <iostream>
#include <stdio.h>
#include <fstream>
using namespace std;

void MyQuoteSpi::OnError(XTPRI *error_info, bool is_last)
{
	cout << "--->>> "
		 << "OnRspError" << endl;
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
	cout << "--->>> "
		 << "OnDisconnected quote" << endl;
	cout << "--->>> Reason = " << reason << endl;
}

void MyQuoteSpi::OnSubMarketData(XTPST *ticker, XTPRI *error_info, bool is_last)
{
	// cout << "OnRspSubMarketData -----" << endl;
}

void MyQuoteSpi::OnUnSubMarketData(XTPST *ticker, XTPRI *error_info, bool is_last)
{
	// cout << "OnRspUnSubMarketData -----------" << endl;
}

void MyQuoteSpi::OnDepthMarketData(XTPMD *market_data, int32_t bid1_qty[], int32_t bid1_count, int32_t max_bid1_count, int32_t ask1_qty[], int32_t ask1_count, int32_t max_ask1_count)
{
	string ticker_info(market_data->ticker);
	XTPDMET xtpdmet_single_data;
	constexpr std::size_t k_max_depth_level = 10;

	map_xtpdmet[ticker_info].vec_depthtime.emplace_back(market_data->data_time);
	// map_xtpdmet[ticker_info].vec_status.emplace_back(market_data->ticker_status);
	map_xtpdmet[ticker_info].vec_overall_price[0].emplace_back(market_data->pre_close_price);
	map_xtpdmet[ticker_info].vec_overall_price[1].emplace_back(market_data->open_price);
	map_xtpdmet[ticker_info].vec_overall_price[2].emplace_back(market_data->high_price);
	map_xtpdmet[ticker_info].vec_overall_price[3].emplace_back(market_data->low_price);
	map_xtpdmet[ticker_info].vec_overall_price[4].emplace_back(market_data->close_price);
	for(std::size_t index = 0; index < k_max_depth_level; index++){
		map_xtpdmet[ticker_info].vec_bidprice[index].emplace_back(market_data->bid[index]);
	}
	for(std::size_t index = 0; index < k_max_depth_level; index++){
		map_xtpdmet[ticker_info].vec_bidvolume[index].emplace_back(market_data->bid_qty[index]);
	}
	for(std::size_t index = 0; index < k_max_depth_level; index++){
		map_xtpdmet[ticker_info].vec_askprice[index].emplace_back(market_data->ask[index]);
	}
	for(std::size_t index = 0; index < k_max_depth_level; index++){
		map_xtpdmet[ticker_info].vec_askvolume[index].emplace_back(market_data->ask_qty[index]);
	}
	map_xtpdmet[ticker_info].vec_trades.emplace_back(market_data->trades_count);
	map_xtpdmet[ticker_info].vec_volume.emplace_back(market_data->qty);
	map_xtpdmet[ticker_info].turnover.emplace_back(market_data->turnover);
	time_t local_time = time(NULL);
	tm *tm_local_time = localtime(&local_time);

	//map_xtpdmet[ticker_info].vec_localtime.emplace_back();
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

void MyQuoteSpi::OnUnSubTickByTick(XTPST *ticker, XTPRI *error_info, bool is_last)
{
}

void MyQuoteSpi::OnOrderBook(XTPOB *order_book)
{
}

void MyQuoteSpi::OnTickByTick(XTPTBT *tbt_data)
{
}

void MyQuoteSpi::OnQueryAllTickers(XTPQSI *ticker_info, XTPRI *error_info, bool is_last)
{
	// cout << "OnQueryAllTickers -----------" << endl;

	if (ticker_info) {
		if (ticker_info->exchange_id == XTP_EXCHANGE_SH) {
			// If starts with ‘6’, sh
			if (ticker_info->ticker[0] == '6')
				ticker_sh.push_back(*ticker_info);

			// After receiving last, print to ticker ids to .txt
			if (is_last) {
				processed_sh = true;
				cv_last.notify_one();
				std::cout << "I'M LAST_SH_TICKER" << std::endl;
			}
		} 
		else if (ticker_info->exchange_id == XTP_EXCHANGE_SZ) {
			// If starts with ‘0|3’, sz
			if (ticker_info->ticker[0] == '0' || ticker_info->ticker[0] == '3') 
				ticker_sz.push_back(*ticker_info);
			
			if (is_last) {
				processed_sz = true;
				cv_last.notify_one();
				std::cout << "I'M LAST_SZ_TICKER" << std::endl;
			}
		}
	}
	else if(error_info) {
		std::cout << "OnQueryAllTickers error( error_id = " << error_info->error_id << ") " 
		<< error_info->error_msg << std::endl;
	}
}

void MyQuoteSpi::OnQueryTickersPriceInfo(XTPTPI *ticker_info, XTPRI *error_info, bool is_last)
{
}

void MyQuoteSpi::OnSubscribeAllMarketData(XTP_EXCHANGE_TYPE exchange_id, XTPRI *error_info)
{
}

void MyQuoteSpi::OnUnSubscribeAllMarketData(XTP_EXCHANGE_TYPE exchange_id, XTPRI *error_info)
{
}

void MyQuoteSpi::OnSubscribeAllOrderBook(XTP_EXCHANGE_TYPE exchange_id, XTPRI *error_info)
{
}

void MyQuoteSpi::OnUnSubscribeAllOrderBook(XTP_EXCHANGE_TYPE exchange_id, XTPRI *error_info)
{
}

void MyQuoteSpi::OnSubscribeAllTickByTick(XTP_EXCHANGE_TYPE exchange_id, XTPRI *error_info)
{
}

void MyQuoteSpi::OnUnSubscribeAllTickByTick(XTP_EXCHANGE_TYPE exchange_id, XTPRI *error_info)
{
}

void MyQuoteSpi::OnSubscribeAllOptionMarketData(XTP_EXCHANGE_TYPE exchange_id, XTPRI *error_info)
{
}

void MyQuoteSpi::OnUnSubscribeAllOptionMarketData(XTP_EXCHANGE_TYPE exchange_id, XTPRI *error_info)
{
}

void MyQuoteSpi::OnSubscribeAllOptionOrderBook(XTP_EXCHANGE_TYPE exchange_id, XTPRI *error_info)
{
}

void MyQuoteSpi::OnUnSubscribeAllOptionOrderBook(XTP_EXCHANGE_TYPE exchange_id, XTPRI *error_info)
{
}

void MyQuoteSpi::OnSubscribeAllOptionTickByTick(XTP_EXCHANGE_TYPE exchange_id, XTPRI *error_info)
{
}

void MyQuoteSpi::OnUnSubscribeAllOptionTickByTick(XTP_EXCHANGE_TYPE exchange_id, XTPRI *error_info)
{
}

bool MyQuoteSpi::IsErrorRspInfo(XTPRI *pRspInfo)
{
	bool bResult = ((pRspInfo) && (pRspInfo->error_id != 0));
	if (bResult)
		cout << "--->>> ErrorID=" << pRspInfo->error_id << ", ErrorMsg=" << pRspInfo->error_msg << endl;
	return bResult;
}
 //TODO
void MyQuoteSpi::print_vec_xtpdmet(const std::map<std::string, XTPDMET> map_xtpdmet, const string file_path) const
{}

/*	char market_data_path[100] = {'\0'};
	sprintf(market_data_path, "%s/_market_data.csv", file_path);
	mkdir_if_not_exist(file_path);
	std::ofstream market_data_outfile;
	market_data_outfile.open(market_data_path, std::ios::out);

	market_data_outfile << "data_time"
						<< ","
						<< "ticker"
						<< ","
						<< "last_price"
						<< ","
						<< "qty"
						<< ","
						<< "turnover"
						<< ","
						<< "bid"
						<< ","
						<< "bit_qty"
						<< ","
						<< "ask"
						<< ","
						<< "ask_qty"
						<< ","
						<< "trades_count"
						<< ","
						<< "local_time" << std::endl;

	constexpr std::size_t k_max_depth_level = 10;
	size_t index;
	time_t local_time = time(NULL);
	tm *tm_local_time = localtime(&local_time);
	std::vector<XTPMD>::iterator iter_xtpmd;
	std::vector<std::string>::iterator iter_time;
	for (iter_xtpmd = vec_xtpmd.begin(), iter_time = vec_localtime.begin();
		 iter_xtpmd < vec_xtpmd.end(); iter_xtpmd++, iter_time++)
	{

		XTPMD market_data = *iter_xtpmd;
		market_data_outfile << market_data.data_time << ","
							<< market_data.ticker << ","
							<< market_data.last_price << ","
							<< market_data.qty << ","
							<< market_data.turnover << ",";

		for (index = 0; index < k_max_depth_level; index++)
		{
			market_data_outfile << market_data.bid[index] << ' ';
		}
		market_data_outfile << ",";

		for (index = 0; index < k_max_depth_level; index++)
		{
			market_data_outfile << market_data.bid_qty[index] << ' ';
		}
		market_data_outfile << ",";

		for (index = 0; index < k_max_depth_level; index++)
		{
			market_data_outfile << market_data.ask[index] << ' ';
		}
		market_data_outfile << ",";

		for (index = 0; index < k_max_depth_level; index++)
		{
			market_data_outfile << market_data.ask_qty[index] << ' ';
		}
		market_data_outfile << ",";

		market_data_outfile << market_data.trades_count << ","
							<< *iter_time;
	}

	market_data_outfile.close();
}
*/
void MyQuoteSpi::print_ticker_info(XTP_EXCHANGE_TYPE exchange_id, const std::string query_ticker_path) const {
	std::ofstream query_ticker_outfile;
	// Old ticker file has been deleted
	// So we can use 'app' to append new query results
	query_ticker_outfile.open(query_ticker_path, std::ios::app);
	
	if(exchange_id == XTP_EXCHANGE_SH)
		for (auto ticker_info : ticker_sh) {
			query_ticker_outfile << ticker_info.ticker << std::endl;
		}
	else if(exchange_id == XTP_EXCHANGE_SZ)
		for (auto ticker_info : ticker_sz) {
			query_ticker_outfile << ticker_info.ticker << std::endl;
		}
	query_ticker_outfile.close();
}