#include "iwd/download_file.hpp"

#include <curl/curl.h>
#include <curl/easy.h>
#include <iomanip>
#include <vn/string_utils.hpp>

namespace iwd {

namespace {
int
progress_callback(
  void* clientp,
  curl_off_t dltotal,
  curl_off_t dlnow,
  curl_off_t ultotal,
  curl_off_t ulnow)
{
  return 0;
}

} // namespace

void
download_file(
  const std::string& url,
  const std::filesystem::path& output,
  const download_settings& settings)
{
  CURL* curl;
  FILE* fp;
  CURLcode res;

  curl = curl_easy_init();

  if (curl) {
    fp = std::fopen(output.c_str(), "wb");
    if (!fp) {
      throw std::runtime_error(vn::make_message(
        "Could not open", output.string(), "file for writing"));
    }
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, NULL);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);
    res = curl_easy_perform(curl);
    curl_easy_cleanup(curl);
    fclose(fp);
  }
}

} // namespace iwd
