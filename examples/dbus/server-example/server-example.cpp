/*
 * Copyright (c) 2011 Samsung Electronics Co., Ltd All Rights Reserved
 *
 *    Licensed under the Apache License, Version 2.0 (the "License");
 *    you may not use this file except in compliance with the License.
 *    You may obtain a copy of the License at
 *
 *        http://www.apache.org/licenses/LICENSE-2.0
 *
 *    Unless required by applicable law or agreed to in writing, software
 *    distributed under the License is distributed on an "AS IS" BASIS,
 *    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *    See the License for the specific language governing permissions and
 *    limitations under the License.
 */
#include <stddef.h>
#include <memory>
#include <iostream>
#include <gio/gio.h>
#include <dpl/event_listener.h>
#include <dpl/log/log.h>
#include <dpl/thread.h>
#include <dpl/dbus/server.h>
#include <dpl/dbus/connection.h>

std::string xml = 
"<node>"
"  <interface name='org.tizen.EchoInterface'>"
"    <method name='echo'>"
"      <arg type='s' name='input' direction='in'/>"
"      <arg type='s' name='output' direction='out'/>"
"    </method>"
"  </interface>"
"  <interface name='org.tizen.QuitInterface'>"
"    <method name='quit'>"
"    </method>"
"  </interface>"
"</node>";

GMainLoop* g_loop = NULL;

auto g_interfaces = DPL::DBus::Interface::fromXMLString(xml);

class DBusDispatcher : public DPL::DBus::Dispatcher
{
public:
    void onMethodCall(GDBusConnection *connection,
                      const gchar *sender,
                      const gchar *objectPath,
                      const gchar *interfaceName,
                      const gchar *methodName,
                      GVariant *parameters,
                      GDBusMethodInvocation *invocation)
    {
          LogDebug("On method call: " << methodName);

          if (g_strcmp0(methodName, "echo") == 0)
          {
              const gchar* arg = NULL;

              g_variant_get(parameters, "(&s)", &arg);
              LogDebug("Client said: " << arg);

              gchar* response = g_strdup_printf(arg);
              g_dbus_method_invocation_return_value(invocation,
                                                    g_variant_new("(s)",
                                                                  response));
              g_free (response);
              sleep(5);
          }
          else if (g_strcmp0(methodName, "quit") == 0)
          {
              g_main_loop_quit(g_loop);
          }
    }
};

class NewConnectionListener :
        public DPL::EventListener<DPL::DBus::ServerEvents::NewConnectionEvent>
{
protected:
    void OnEventReceived(const DPL::DBus::ServerEvents::NewConnectionEvent& event)
    {
        m_connection = event.GetArg0();

        auto quitInterface = g_interfaces.at(1);
        quitInterface->setDispatcher(std::make_shared<DBusDispatcher>());
        m_quitObject = DBus::Object::create("/object/quit", quitInterface);

        m_connection->registerObject(m_quitObject);
    }

private:
    DPL::DBus::ConnectionPtr m_connection;
    DPL::DBus::ObjectPtr m_quitObject;
};

int main()
{
  g_type_init();

  // --------------- echo
  auto echoConnection = DPL::DBus::Connection::sessionBus();

  auto echoInterface = g_interfaces.at(0);
  echoInterface->setDispatcher(std::make_shared<DBusDispatcher>());

  auto echoObject = DPL::DBus::Object::create("/object/echo", echoInterface);

  echoConnection->registerObject(echoObject);

  echoConnection->registerService("org.tizen.EchoService");

  // --------------- quit
  std::unique_ptr<NewConnectionListener> listener(new NewConnectionListener);
  auto server = DPL::DBus::Server::create("unix:abstract=someaddr");
  server->AddListener(listener.get());
  server->start();

  g_loop = g_main_loop_new(NULL, FALSE);
  g_main_loop_run(g_loop);
  g_main_loop_unref(g_loop);

  return 0;
}
