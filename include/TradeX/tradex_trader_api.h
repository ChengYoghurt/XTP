/////////////////////////////////////////////////////////////////////////
///@system  TradeX新一代交易系统
///@company 华创证券有限责任公司
///@brief   定义客户端交易接口
///@author  cuiyc
/////////////////////////////////////////////////////////////////////////

#ifndef _TRADEX_API_TRADER_
#define _TRADEX_API_TRADER_

#if _MSC_VER > 1000
#pragma once
#endif

#include "tradex_types.h"
#include "tradex_messages.h"

#if defined(ISLIB) && defined(WIN32)
#ifdef LIB_TRADER_API_EXPORT
#define TRADER_API_EXPORT __declspec(dllexport)
#else
#define TRADER_API_EXPORT __declspec(dllimport)
#endif
#else
#define TRADER_API_EXPORT
#endif

namespace com
{
namespace tradex
{
namespace api
{

class TradeXSpi
{
public:
    /// 登录响应回调，调用方应在登录成功之后进行后续操作
    virtual void OnLogin(const TRXLoginResponse *loginResponse) = 0;
    /// 注销响应回调
    virtual void OnLogout(const bool isLogoutSuccess, const char *error_message) = 0;
    /// 连接建立成功
    /// <param name="severType">后端服务器类型</param>
    /// <param name="url">网络连接地址</param>
    virtual void OnConnected(const TRXServerType severType, const char *url) = 0;
    /// 连接断开通知
    /// <param name="severType">断开连接的服务器类型</param>
    /// <param name="url">断开连接的地址</param>
    /// <param name="errorMessage">断开原因</param>
    virtual void OnDisconnected(const TRXServerType severType, const char *url, const char *error_message) = 0;
    /// 委托回报响应
    /// <param name="orderReport">返回的委托信息</param>
    virtual void OnOrderEvent(const TRXOrderReport *orderReport) = 0;
    /// 返回的成交回报信息
    /// <param name="tradeReport"></param>
    virtual void OnTradeEvent(const TRXTradeReport *tradeReport) = 0;
    /// 交易单元连接状态通知
    /// <param name="notice"></param>
    virtual void OnTradeUnitStatus(const TRXTradeUnitConnStatusNotice *notice) = 0;
    /// 撤单被拒绝回报
    /// <param name="cancelReject"></param>
    virtual void OnCancelReject(const TRXOrderCancelReject *cancelReject) = 0;
    /// 持仓查询返回结果
    /// <param name="positionsList"></param>
    virtual void OnQueryPosition(const TRXPosition *position, const text_t error_message, const request_id_t request_id, const bool is_last, const bool is_success = true) = 0;
    /// 标准券持仓查询返回结果
    /// <param name="positionsList"></param>
    virtual void OnQueryStandardCouponPosition(const TRXPosition *position, const text_t error_message, const request_id_t request_id, const bool is_last, const bool is_success = true) = 0;
    /// 账户资金查询返回结果
    /// <param name="balanceList"></param>
    virtual void OnQueryBalance(const TRXBalance *balance, const text_t error_message, const request_id_t request_id, const bool is_last, const bool is_success = true) = 0;
    /// 委托查询返回结果
    /// <param name="orderReportList"></param>
    virtual void OnQueryOrder(const TRXOrderReport *order, const text_t error_message, const request_id_t request_id, const bool is_last, const bool is_success = true) = 0;
    /// 成交回报查询返回结果
    /// <param name="tradeReportList"></param>
    virtual void OnQueryTrade(const TRXTradeReport *tradeReport, const text_t error_message, const request_id_t request_id, const bool is_last, const bool is_success = true) = 0;
    /// 可融券查询返回结果
    /// <param name="borrowingSecurityList"></param>
    virtual void OnQueryBorrowingSecurity(const TRXBorrowingSecurity *borrowingSecurity, const text_t error_message, const request_id_t request_id, const bool is_last, const bool is_success = true) = 0;
    /// 融券负债查询返回结果
    /// <param name="securityLiabilityList"></param>
    virtual void OnQuerySecurityLiability(const TRXSecurityLiability *securityLiability, const text_t error_message, const request_id_t request_id, const bool is_last, const bool is_success = true) = 0;
    /// 融资负债查询返回结果
    /// <param name="financingLiabilityList"></param>
    virtual void OnQueryFinancingLiability(const TRXFinancingLiability *financingLiability, const text_t error_message, const request_id_t request_id, const bool is_last, const bool is_success = true) = 0;
    /// 信用资产查询返回结果
    /// <param name="creditAssetList"></param>
    virtual void OnQueryCreditAsset(const TRXCreditAsset *creditAsset, const text_t error_message, const request_id_t request_id, const bool is_last, const bool is_success = true) = 0;
};

class TRADER_API_EXPORT TradeXApi
{
public:
    ///创建TradeXApi对象，必须指定一个日志存放路径，多次调用以第一次的日志路径为准
    static TradeXApi *CreateTraderApi(const char *save_file_path, TRXLogLevel log_level = TRXLogLevel::TRX_INFO);
    ///通过CreateTraderApi必须调用该方法销毁对象
    virtual void Release() = 0;
    ///获取当前交易日
    ///只有登录成功后,才能得到正确的交易日
    virtual const char *GetTradingDay() = 0;
    ///注册回调接口
    ///spi 派生自回调接口类的实例，请在登录之前设定
    virtual void RegisterSpi(const TradeXSpi *spi) = 0;
    ///登录请求
    virtual int Login(const TRXLoginRequest *request) = 0;
    ///注销请求
    virtual int Logout() = 0;
    ///报单个订单
    virtual int PlaceOrder(const TRXSingleOrder *order) = 0;
    ///报篮子委托
    virtual int PlaceBasketOrder(const TRXBasketOrder *basket) = 0;
    ///撤销单个订单
    virtual int CancelOrder(const TRXOrderCancelRequest *request) = 0;
    ///撤销整个篮子委托
    virtual int CancelBasketOrder(const TRXBasketCancelRequest *request) = 0;
    ///查询订单
    virtual int QueryOrder(const TRXOrderQueryRequest *request) = 0;
    ///查询资金
    virtual int QueryBalance(const TRXBalanceQueryRequest *request) = 0;
    ///查询持仓
    virtual int QueryPosition(const TRXPositionQueryRequest *request) = 0;
    ///查询标准券持仓
    virtual int QueryStandardCouponPosition(const TRXPositionQueryRequest *request) = 0;
    ///查询成交信息
    virtual int QueryTrade(const TRXTradeQueryRequest *request) = 0;
    ///查询信用资产信息
    virtual int QueryCreditAsset(const TRXCreditAssetQueryRequest *request) = 0;
    ///查询融券负债
    virtual int QuerySecurityLiability(const TRXSecurityLiabilityQueryRequest *request) = 0;
    ///查询融资负债
    virtual int QueryFinancingLiability(const TRXFinancingLiabilityQueryRequest *request) = 0;
    ///查询可融券信息
    virtual int QueryBorrowingSecurity(const TRXBorrowingSecurityQueryRequest *request) = 0;
    ///查询交易单元状态
    virtual int QueryTradeUnitStatus(const TRXTradeUnitStatusQueryRequest *request) = 0;
};

} // namespace api
} // namespace tradex
} // namespace com

#endif