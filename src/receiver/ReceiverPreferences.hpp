#include <map>



class ReceiverPreferences{
  using preferences_t = std::map<IPackageReceiver*, double>;
  using const_iterator = preferences_t::const_iterator; 

  private:
    preferences_t pref;
    ReceiverPreferences(ProbabilityGenerator);
    void add_receiver(IPackageReceiver*);
    void remove_receiver(IPackageReceiver*);
    IPackageReceiver* choose_receiver(void);
    preferences_t& get_preferences(void) const;
}