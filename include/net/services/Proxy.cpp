#include "../Daemon.hpp"

namespace ICSMeter
{
  namespace net
  {

    namespace proxy
    {

      using namespace daemon;
      using namespace UI;

      civ_subscription_t CIV_CHECK_SUB          = { "Ping",   &CIV::CIV_CHECK,         6, 5,   0, false };
      civ_subscription_t CIV_GET_TX_SUB         = { "TX",     &CIV::CIV_GET_TX,        5, 1,   0, false };
      civ_subscription_t CIV_GET_DATA_MODE_SUB  = { "DAT",    &CIV::CIV_GET_DATA_MODE, 6, 1,   0, false };
      civ_subscription_t CIV_GET_SMETER_SUB     = { "SMeter", &CIV::CIV_GET_SMETER,    6, 0.5, 0, false };
      civ_subscription_t CIV_GET_PWR_SUB        = { "PWR",    &CIV::CIV_GET_PWR,       6, 0.5, 0, false };
      civ_subscription_t CIV_GET_SWR_SUB        = { "SWR",    &CIV::CIV_GET_SWR,       6, 0.5, 0, false };
      civ_subscription_t CIV_GET_FRQ_SUB        = { "FRQ",    &CIV::CIV_GET_FRQ,       8, 1,   0, false };
      civ_subscription_t CIV_GET_MOD_SUB        = { "MOD",    &CIV::CIV_GET_MOD,       5, 1,   0, false };

      civ_subscription_t *meter_subscriptions[3] =
      {
        &CIV_GET_PWR_SUB,
        &CIV_GET_SMETER_SUB,
        &CIV_GET_SWR_SUB
      };

      civ_subscription_t *meter_subscription = meter_subscriptions[Measure::mode]; // pointer to currently selected meter mode


      civ_subscription_t *subscriptions[] =
      {
        &CIV_CHECK_SUB,
        &CIV_GET_TX_SUB,
        &CIV_GET_DATA_MODE_SUB,
        meter_subscription,
        &CIV_GET_FRQ_SUB,
        &CIV_GET_MOD_SUB
      };


      size_t subscriptions_count = sizeof( subscriptions ) / sizeof( subscriptions[0] );
      bool online = false;

      void setup()
      {
        //checkStatus();
      }


      bool available()
      {
        return online;
      }


      bool connected()
      {
        return CIV::txConnected;
      }


      void setFlag( proxy_flags_t flag )
      {
        switch( flag ) {
          case PROXY_ONLINE:  online = true;       break;
          case PROXY_OFFLINE: online = false;      break;
          case TX_ONLINE:     CIV::txConnected = true; CIV::had_success = true; message = nullptr; break;
          case TX_OFFLINE:    CIV::txConnected = false; break;
        }
      }


    };

  };

};

