/*

Examples:

// Test with GET over HTTPS
std::string response_str = http_request("GET", "https://httpbin.org/get");
std::cout << response_str << std::endl;

// Test with POST form data over HTTP
auto *form = new Poco::Net::HTMLForm();
form->set("a", "b");
std::string response_str2 = http_request("POST", "http://httpbin.org/post", HttpBody(form));
std::cout << response_str2 << std::endl;

// Test with POST and string payload over HTTPS
std::string response_str3 = http_request("POST", "https://httpbin.org/post", HttpBody("asdfr"));
std::cout << response_str3 << std::endl;

*/

#pragma once

#include <iostream>
#include <memory>
#include <stdexcept>

#include <Poco/URI.h>
#include <Poco/Net/HTTPSClientSession.h>
#include <Poco/Net/HTTPClientSession.h>
#include <Poco/Net/HTTPRequest.h>
#include <Poco/Net/HTTPResponse.h>
#include <Poco/Net/HTMLForm.h>

enum class HttpBodyType {
  NONE,
  STRING,
  FORM
};

class HttpBody {
public:
  HttpBody() = default;

  HttpBody(std::string payload_str) : payload_str(payload_str), body_type(HttpBodyType::STRING) {}

  HttpBody(Poco::Net::HTMLForm *form) : form(std::unique_ptr<Poco::Net::HTMLForm>(form)), body_type(HttpBodyType::FORM) {}

  HttpBodyType body_type = HttpBodyType::NONE;

  std::string payload_str = "";

  std::unique_ptr<Poco::Net::HTMLForm> form;
};

//Creates, sends, receives the http request and returns the response string
template <class T>
inline std::string http_request_impl(T &session, Poco::Net::HTTPRequest &request, Poco::Net::HTTPResponse &response, HttpBody &&body, bool ignore_err) {
  
  //Add the payload depending on what type it is
  if (body.body_type == HttpBodyType::FORM) {
    body.form->prepareSubmit(request);
  } else if (body.body_type == HttpBodyType::STRING) {
    request.setContentLength(body.payload_str.size());
  }

  //Now send the request
  std::ostream &rq = session.sendRequest(request);
  if (body.body_type == HttpBodyType::FORM) {
    body.form->write(rq);
  } else if (body.body_type == HttpBodyType::STRING) {
    rq << body.payload_str;
    rq.flush();
  }

  //Receive response and return response string
  std::istream &rs = session.receiveResponse(response);
  
  int status = response.getStatus();
  if ((status < 200 || status >= 400) && !ignore_err) {
    throw std::runtime_error("Request failed");
  }
  std::string response_str;
  std::string line;
  while (std::getline(rs, line)) {
    response_str += line + "\n";
  }
  return response_str;
}

inline std::string http_method_to_poco_method(std::string method) {
  if (method == "GET")
    return Poco::Net::HTTPRequest::HTTP_GET;
  if (method == "POST")
    return Poco::Net::HTTPRequest::HTTP_POST;
  if (method == "PUT")
    return Poco::Net::HTTPRequest::HTTP_POST;
  throw std::runtime_error("Unknown HTTP method");
}

//Makes the http/s request
inline std::string http_request(std::string method, std::string uri_str, HttpBody &&body = HttpBody(), bool ignore_err = false)
{
  // Get the method
  std::string poco_method = http_method_to_poco_method(method);
  Poco::URI uri { uri_str }; // List initalization
  std::string path(uri.getPathAndQuery());
  if (path.empty()) path = "/";

  //Create res and req objeects
  Poco::Net::HTTPResponse response;
  Poco::Net::HTTPRequest request(poco_method, path, Poco::Net::HTTPMessage::HTTP_1_1);

  //Send
  if (uri.getScheme() == "http") {
    Poco::Net::HTTPClientSession session(uri.getHost(), uri.getPort());
    return http_request_impl(session, request, response, std::move(body), ignore_err);
  } else if (uri.getScheme() == "https") {
    Poco::Net::HTTPSClientSession session(uri.getHost(), uri.getPort());
    return http_request_impl(session, request, response, std::move(body), ignore_err);
  } else {
    throw std::runtime_error("Unknown request scheme");
  }
}
