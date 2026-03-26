#include "taxi_storage.hpp"

#include <algorithm>
#include <cctype>

namespace taxi {

namespace {

std::string MakeToken(int user_id, int token_id) {
  return "token-" + std::to_string(user_id) + "-" + std::to_string(token_id);
}

}  // namespace

std::optional<User> TaxiStorage::CreateUser(const std::string& login,
                                            const std::string& password,
                                            const std::string& full_name) {
  std::lock_guard<std::mutex> lock(mutex_);

  for (const auto& [id, user] : users_) {
    if (user.login == login) {
      return std::nullopt;
    }
  }

  User user;
  user.id = next_user_id_++;
  user.login = login;
  user.password = password;
  user.full_name = full_name;

  users_.emplace(user.id, user);
  return user;
}

std::optional<User> TaxiStorage::GetUserById(int user_id) const {
  std::lock_guard<std::mutex> lock(mutex_);

  auto it = users_.find(user_id);
  if (it == users_.end()) {
    return std::nullopt;
  }

  return it->second;
}

std::optional<User> TaxiStorage::GetUserByLogin(const std::string& login) const {
  std::lock_guard<std::mutex> lock(mutex_);

  for (const auto& [id, user] : users_) {
    if (user.login == login) {
      return user;
    }
  }

  return std::nullopt;
}

std::vector<User> TaxiStorage::FindUsersByNameMask(
    const std::string& name_mask) const {
  std::lock_guard<std::mutex> lock(mutex_);

  std::vector<User> result;
  for (const auto& [id, user] : users_) {
    if (ContainsCaseInsensitive(user.full_name, name_mask)) {
      result.push_back(user);
    }
  }

  return result;
}

std::optional<std::string> TaxiStorage::Login(const std::string& login,
                                              const std::string& password) {
  std::lock_guard<std::mutex> lock(mutex_);

  for (const auto& [id, user] : users_) {
    if (user.login == login && user.password == password) {
      const std::string token = MakeToken(user.id, next_token_id_++);
      tokens_[token] = user.id;
      return token;
    }
  }

  return std::nullopt;
}

bool TaxiStorage::IsTokenValid(const std::string& token) const {
  std::lock_guard<std::mutex> lock(mutex_);
  return tokens_.find(token) != tokens_.end();
}

std::optional<Driver> TaxiStorage::CreateDriver(
    int user_id,
    const std::string& car_model,
    const std::string& car_number,
    const std::string& license_number) {
  std::lock_guard<std::mutex> lock(mutex_);

  if (users_.find(user_id) == users_.end()) {
    return std::nullopt;
  }

  for (const auto& [id, driver] : drivers_) {
    if (driver.user_id == user_id) {
      return std::nullopt;
    }
  }

  Driver driver;
  driver.id = next_driver_id_++;
  driver.user_id = user_id;
  driver.car_model = car_model;
  driver.car_number = car_number;
  driver.license_number = license_number;
  driver.status = "online";

  drivers_.emplace(driver.id, driver);
  return driver;
}

std::optional<Driver> TaxiStorage::GetDriverById(int driver_id) const {
  std::lock_guard<std::mutex> lock(mutex_);

  auto it = drivers_.find(driver_id);
  if (it == drivers_.end()) {
    return std::nullopt;
  }

  return it->second;
}

std::optional<Driver> TaxiStorage::GetDriverByUserId(int user_id) const {
  std::lock_guard<std::mutex> lock(mutex_);

  for (const auto& [id, driver] : drivers_) {
    if (driver.user_id == user_id) {
      return driver;
    }
  }

  return std::nullopt;
}

std::vector<Driver> TaxiStorage::GetActiveDrivers() const {
  std::lock_guard<std::mutex> lock(mutex_);

  std::vector<Driver> result;
  for (const auto& [id, driver] : drivers_) {
    if (driver.status == "online") {
      result.push_back(driver);
    }
  }

  return result;
}

std::optional<Driver> TaxiStorage::UpdateDriverStatus(
    int driver_id,
    const std::string& status) {
  std::lock_guard<std::mutex> lock(mutex_);

  auto it = drivers_.find(driver_id);
  if (it == drivers_.end()) {
    return std::nullopt;
  }

  it->second.status = status;
  return it->second;
}

std::optional<Ride> TaxiStorage::CreateRide(
    int passenger_id,
    const std::string& pickup_address,
    const std::string& destination_address) {
  std::lock_guard<std::mutex> lock(mutex_);

  if (users_.find(passenger_id) == users_.end()) {
    return std::nullopt;
  }

  Ride ride;
  ride.id = next_ride_id_++;
  ride.passenger_id = passenger_id;
  ride.driver_id = 0;
  ride.pickup_address = pickup_address;
  ride.destination_address = destination_address;
  ride.status = "searching";

  rides_.emplace(ride.id, ride);
  return ride;
}

std::optional<Ride> TaxiStorage::GetRideById(int ride_id) const {
  std::lock_guard<std::mutex> lock(mutex_);

  auto it = rides_.find(ride_id);
  if (it == rides_.end()) {
    return std::nullopt;
  }

  return it->second;
}

std::vector<Ride> TaxiStorage::GetRidesByUserId(int user_id) const {
  std::lock_guard<std::mutex> lock(mutex_);

  std::vector<Ride> result;
  for (const auto& [id, ride] : rides_) {
    if (ride.passenger_id == user_id) {
      result.push_back(ride);
    }
  }

  return result;
}

std::vector<Ride> TaxiStorage::GetActiveRides() const {
  std::lock_guard<std::mutex> lock(mutex_);

  std::vector<Ride> result;
  for (const auto& [id, ride] : rides_) {
    if (ride.status == "searching" || ride.status == "accepted") {
      result.push_back(ride);
    }
  }

  return result;
}

std::optional<Ride> TaxiStorage::AcceptRide(int ride_id, int driver_id) {
  std::lock_guard<std::mutex> lock(mutex_);

  auto ride_it = rides_.find(ride_id);
  if (ride_it == rides_.end()) {
    return std::nullopt;
  }

  auto driver_it = drivers_.find(driver_id);
  if (driver_it == drivers_.end()) {
    return std::nullopt;
  }

  if (ride_it->second.status != "searching") {
    return std::nullopt;
  }

  if (driver_it->second.status != "online") {
    return std::nullopt;
  }

  ride_it->second.driver_id = driver_id;
  ride_it->second.status = "accepted";
  driver_it->second.status = "busy";

  return ride_it->second;
}

std::optional<Ride> TaxiStorage::CompleteRide(int ride_id) {
  std::lock_guard<std::mutex> lock(mutex_);

  auto ride_it = rides_.find(ride_id);
  if (ride_it == rides_.end()) {
    return std::nullopt;
  }

  if (ride_it->second.status != "accepted") {
    return std::nullopt;
  }

  ride_it->second.status = "completed";

  auto driver_it = drivers_.find(ride_it->second.driver_id);
  if (driver_it != drivers_.end()) {
    driver_it->second.status = "online";
  }

  return ride_it->second;
}

std::string TaxiStorage::ToLower(std::string value) {
  std::transform(value.begin(), value.end(), value.begin(),
                 [](unsigned char ch) {
                   return static_cast<char>(std::tolower(ch));
                 });
  return value;
}

bool TaxiStorage::ContainsCaseInsensitive(const std::string& haystack,
                                          const std::string& needle) {
  const std::string lower_haystack = ToLower(haystack);
  const std::string lower_needle = ToLower(needle);

  return lower_haystack.find(lower_needle) != std::string::npos;
}

}  // namespace taxi
