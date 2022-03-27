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

void MyQuoteSpi::OnDepthMarketData(XTPMD *market_data, int64_t bid1_qty[], int32_t bid1_count, int32_t max_bid1_count, int64_t ask1_qty[], int32_t ask1_count, int32_t max_ask1_count)
{
	//store market data into map_xtpmdet_ 
	string ticker_info(market_data->ticker);
	constexpr std::size_t k_max_depth_level = 10;
	map_xtpdmet_[ticker_info].vec_depthtime.emplace_back(market_data->data_time);
	map_xtpdmet_[ticker_info].vec_status.emplace_back(market_data->ticker_status);
	map_xtpdmet_[ticker_info].vec_overall_price[0].emplace_back(market_data->pre_close_price);
	map_xtpdmet_[ticker_info].vec_overall_price[1].emplace_back(market_data->open_price);
	map_xtpdmet_[ticker_info].vec_overall_price[2].emplace_back(market_data->high_price);
	map_xtpdmet_[ticker_info].vec_overall_price[3].emplace_back(market_data->low_price);
	map_xtpdmet_[ticker_info].vec_overall_price[4].emplace_back(market_data->close_price);
	for(std::size_t index = 0; index < k_max_depth_level; index++){
		map_xtpdmet_[ticker_info].vec_bidprice[index].emplace_back(market_data->bid[index]);
	}
	for(std::size_t index = 0; index < k_max_depth_level; index++){
		map_xtpdmet_[ticker_info].vec_bidvolume[index].emplace_back(market_data->bid_qty[index]);
	}
	for(std::size_t index = 0; index < k_max_depth_level; index++){
		map_xtpdmet_[ticker_info].vec_askprice[index].emplace_back(market_data->ask[index]);
	}
	for(std::size_t index = 0; index < k_max_depth_level; index++){
		map_xtpdmet_[ticker_info].vec_askvolume[index].emplace_back(market_data->ask_qty[index]);
	}
	map_xtpdmet_[ticker_info].vec_trades.emplace_back(market_data->trades_count);
	map_xtpdmet_[ticker_info].vec_volume.emplace_back(market_data->qty);
	map_xtpdmet_[ticker_info].vec_turnover.emplace_back(market_data->turnover);
	l2agg::timestamp_t localtime = kf::NumericTime::now();
	map_xtpdmet_[ticker_info].vec_localtime.emplace_back(localtime);
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
				std::cout << "I'M LAST_SH_TICKER" << std::endl;
			}
		} 
		else if (ticker_info->exchange_id == XTP_EXCHANGE_SZ) {
			// If starts with ‘0|3’, sz
			if (ticker_info->ticker[0] == '0' || ticker_info->ticker[0] == '3') 
				ticker_sz.push_back(*ticker_info);
			
			if (is_last) {
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
void MyQuoteSpi::print_vec_xtpdmet(const std::string &file_path) const
{
	//print function, tickers split with #  
	char market_data_path[100] = {'\0'};
	sprintf(market_data_path, "%s/_market_data.csv", file_path.c_str());
	mkdir_if_not_exist(file_path);
	std::ofstream market_data_outfile;
	std::cout << market_data_path << std::endl;
	market_data_outfile.open(market_data_path, std::ios::out);
	constexpr std::size_t k_max_depth_level = 10;
	size_t index;
	
	for(auto&item_xtpdmet : map_xtpdmet_){
		market_data_outfile << "ticker" << ","
							<< item_xtpdmet.first <<std::endl
							<< "depthtime";
		for(auto &item_vec : item_xtpdmet.second.vec_depthtime){
			market_data_outfile << "," << item_vec ;
		}
		market_data_outfile << std::endl
							<< "turnover";
		for(auto &item_vec : item_xtpdmet.second.vec_turnover){
			market_data_outfile << "," << item_vec ;
		}
		market_data_outfile << std::endl
							<< "qty";
		for(auto &item_vec : item_xtpdmet.second.vec_volume){
			market_data_outfile << "," << item_vec ;
		}
		market_data_outfile << std::endl
							<< "precloseprice";
		for(auto &item_vec : item_xtpdmet.second.vec_overall_price[0]){
			market_data_outfile << "," << item_vec ;
		}
		market_data_outfile << std::endl
							<< "openprice";
		for(auto &item_vec : item_xtpdmet.second.vec_overall_price[1]){
			market_data_outfile << "," << item_vec ;
		}
		market_data_outfile << std::endl
							<< "highprice";
		for(auto &item_vec : item_xtpdmet.second.vec_overall_price[2]){
			market_data_outfile << "," << item_vec ;
		}
		market_data_outfile << std::endl
							<< "lowprice";
		for(auto &item_vec : item_xtpdmet.second.vec_overall_price[3]){
			market_data_outfile << "," << item_vec ;
		}
		market_data_outfile << std::endl
							<< "closeprice";
		for(auto &item_vec : item_xtpdmet.second.vec_overall_price[4]){
			market_data_outfile << "," << item_vec ;
		}
		market_data_outfile << std::endl;
		for (index = 0; index < k_max_depth_level; index++)
		{
			market_data_outfile << "bidprice" << index;
			for(auto &item_vec : item_xtpdmet.second.vec_bidprice[index]){
				market_data_outfile << "," << item_vec ;
			}
			market_data_outfile << std::endl;
		}
		for (index = 0; index < k_max_depth_level; index++)
		{
			market_data_outfile << "bidvolume" << index;
			for(auto &item_vec : item_xtpdmet.second.vec_bidvolume[index]){
				market_data_outfile << "," << item_vec ;
			}
			market_data_outfile << std::endl;
		}
		for (index = 0; index < k_max_depth_level; index++)
		{
			market_data_outfile << "askprice" << index;
			for(auto &item_vec : item_xtpdmet.second.vec_askprice[index]){
				market_data_outfile << "," << item_vec ;
			}
			market_data_outfile << std::endl;
		}
		for (index = 0; index < k_max_depth_level; index++)
		{
			market_data_outfile << "askvolume" << index;
			for(auto &item_vec : item_xtpdmet.second.vec_askvolume[index]){
				market_data_outfile << "," << item_vec ;
			}
			market_data_outfile << std::endl;
		}
		market_data_outfile << "localtime";
		for(auto &item_vec : item_xtpdmet.second.vec_localtime){
			market_data_outfile << "," << item_vec ;
		}
		market_data_outfile << std::endl;
		market_data_outfile << '#' << std::endl;

	}
	market_data_outfile.close();
}

void MyQuoteSpi::print_ticker_info(const int& print_type, const char* query_ticker_path) const{
	std::ofstream query_ticker_outfile;
	// Old ticker file has been deleted
	// So we can use 'app' to append new query results
	query_ticker_outfile.open(query_ticker_path, std::ios::app);
	
	if(!print_type)
		for (auto ticker_info : ticker_sh) {
			query_ticker_outfile << ticker_info.ticker << std::endl;
		}
	else
		for (auto ticker_info : ticker_sz) {
			query_ticker_outfile << ticker_info.ticker << std::endl;
		}
	query_ticker_outfile.close();
}