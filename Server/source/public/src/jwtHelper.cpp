
#include "jwt-cpp/jwt.h"

const std::string jwt_secret = "tankserver_jwt_233333";
const std::string jwt_iss = "tankserver_loginservice";

std::string GnenerateJwtStr(const std::string &token)
{

    std::string jwttoken = jwt::create()
                               .set_issuer(jwt_iss)
                               .set_type("JWT")
                               .set_payload_claim("token", jwt::claim(token))
                               .sign(jwt::algorithm::hs256{jwt_secret});

    return jwttoken;
}

bool VerfiyJwtToken(const std::string &jwtstr, std::string &token)
{
    auto decoded = jwt::decode(jwtstr);
    auto verifier = jwt::verify()
                        .allow_algorithm(jwt::algorithm::hs256{jwt_secret})
                        .with_issuer(jwt_iss);
    try
    {
        verifier.verify(decoded);
    }
    catch (const jwt::error::token_verification_exception &e)
    {
        std::cout << "Invalid jwt token: " << e.what() << std::endl;
        return false;
    }
    try
    {
        if (!decoded.has_payload_claim("token"))
            return false;
        jwt::claim token_claim = decoded.get_payload_claim("token");
        if (token_claim.get_type() != jwt::json::type::string)
            return false;
        token = token_claim.as_string();
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error when get payload from jwt: " << e.what() << std::endl;
        return false;
    }
    return true;
}