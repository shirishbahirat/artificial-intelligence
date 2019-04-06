#include <systemc.h>

using namespace sc_core;

// g++ -I. -I$SYSTEMC_HOME/include -L. -L$SYSTEMC_HOME/lib-linux64 -o out
// zones.cpp -lsystemc -lm -DSC_INCLUDE_DYNAMIC_PROCESSES

class sample_module;

class sample_module : public sc_module {
public:
  sc_signal<bool> clock_signal;
  sc_signal<int> count_signal;

  sample_module(sc_module_name nm);
  ~sample_module() {}

  SC_HAS_PROCESS(sample_module);

private:
  virtual void log();
  virtual void run();
};

sample_module::sample_module(sc_module_name nm) : sc_module(nm) {
  SC_THREAD(run);
  SC_METHOD(log);
  sensitive << clock_signal.posedge_event();
}

void sample_module::log() {
  count_signal.write(count_signal.read() + 1);
  if (count_signal.read() >= 1e6) {
    cout << "Reached max count" << endl;
    count_signal.write(0);
  }
}

void sample_module::run() {
  while (true) {
    clock_signal.write(0);
    wait(10, SC_NS);
    clock_signal.write(1);
    wait(10, SC_NS);
  }
}

class spawn_module : public sc_module {

public:
  sc_signal<bool> clock_signal;

  SC_HAS_PROCESS(spawn_module);

  spawn_module(sc_module_name nm);
  ~spawn_module() {}

private:
  void spawned_th(int value);

  void run();
};

spawn_module::spawn_module(sc_module_name nm) : sc_module(nm) {
  SC_THREAD(run);
}

void spawn_module::spawned_th(int value) {
  cout << "spawned_th successfuly spawned with value: " << value << endl;

  while (true) {
    wait(10, SC_NS);
    cout << "spawned_th successfuly spawned with value: " << value << endl;
  }
}

void spawn_module::run() {

  for (int i = 0; i < 10; ++i) {
    sc_spawn(sc_bind(&spawn_module::spawned_th, this, i));
  }

  while (true) {
    clock_signal.write(0);
    wait(10, SC_NS);
    clock_signal.write(1);
    wait(10, SC_NS);
  }
}

int sc_main(int argc, char **argv) {
  sample_module *sm = new sample_module("my_module");
  spawn_module *sw = new spawn_module("expt");
  sc_start();
  return 0;
}
