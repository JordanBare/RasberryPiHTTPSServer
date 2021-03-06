//
// Created by Jordan Bare on 6/4/18.
//

#ifndef SERVER_HTTPWORKER_H
#define SERVER_HTTPWORKER_H

#include <boost/asio/ip/tcp.hpp>
#include <boost/beast.hpp>
#include <boost/asio/ssl/context.hpp>
#include <boost/asio/ssl/stream.hpp>
#include <boost/asio/strand.hpp>
#include "CSRFTokenManager.h"
#include "BlogManager.h"
#include "CredentialsManager.h"

class Session : public std::enable_shared_from_this<Session> {
public:
    Session(boost::asio::ssl::context& sslContext, boost::asio::ip::tcp::socket socket, std::unique_ptr<CSRFTokenManager> &csrfManager, std::unique_ptr<BlogManager> &blogManager, std::unique_ptr<CredentialsManager> &credentialsManager, const std::string &pageRootFolder);
    void run();
    ~Session();
private:
    void onHandshake(boost::system::error_code ec);
    void readRequest();
    void handleReadRequest(boost::system::error_code ec, std::size_t bytes_transferred);
    void processRequest();
    void createGetResponse();
    void createPostResponse();
    bool forbiddenCheck() const;
    void writeResponse();
    void handleWriteResponse(boost::system::error_code ec, std::size_t bytes_transferred);
    void shutdown();
    void onShutdown(boost::system::error_code ec);
    void printErrorCode(boost::beast::error_code &ec);
    std::string readFile(const std::string &resourceFilePath) const;
    void startResponseTimer();
    void onResponseTimerFinish(boost::system::error_code ec);
    void startShutdownTimer();
    void onShutdownTimerFinish(boost::system::error_code ec);

    boost::asio::ip::tcp::socket mSocket;
    boost::asio::ssl::stream<boost::asio::ip::tcp::socket&> mStream;
    boost::asio::strand<boost::asio::io_context::executor_type> mStrand;
    const std::string mPageRoot;
    bool mAuthorized;
    std::unique_ptr<CSRFTokenManager> &mCSRFManager;
    std::unique_ptr<BlogManager> &mBlogManager;
    std::unique_ptr<CredentialsManager> &mCredentialsManager;
    boost::asio::basic_waitable_timer<std::chrono::steady_clock> mResponseDeadline;
    boost::asio::basic_waitable_timer<std::chrono::steady_clock> mShutdownDeadline;
    std::string mCSRFToken;
    //previous flat is 8192
    boost::beast::flat_buffer mBuffer{8192};
    boost::beast::http::request<boost::beast::http::string_body> mRequest;
    boost::beast::http::response<boost::beast::http::dynamic_body> mResponse;

    void fillResponseBodyWithFile(const std::string &resourceFilePath);
};

#endif //SERVER_HTTPWORKER_H
