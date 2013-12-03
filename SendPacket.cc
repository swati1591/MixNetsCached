/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */
     /*
       * Copyright (c) 2009 University of Washington
       *
       * This program is free software; you can redistribute it and/or modify
       * it under the terms of the GNU General Public License version 2 as
       * published by the Free Software Foundation;
       *
       * This program is distributed in the hope that it will be useful,
      * but WITHOUT ANY WARRANTY; without even the implied warranty of
      * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
      * GNU General Public License for more details.
      *
      * You should have received a copy of the GNU General Public License
      * along with this program; if not, write to the Free Software
      * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
      *
      */
     
     //
     // This program configures a grid (default 5x5) of nodes on an 
     // 802.11b physical layer, with
     // 802.11b NICs in adhoc mode, and by default, sends one packet of 1000 
     // (application) bytes to node 1.
     //
     // The default layout is like this, on a 2-D grid.
     //
     // n20  n21  n22  n23  n24
     // n15  n16  n17  n18  n19
     // n10  n11  n12  n13  n14
     // n5   n6   n7   n8   n9
     // n0   n1   n2   n3   n4
     //
     // the layout is affected by the parameters given to GridPositionAllocator;
     // by default, GridWidth is 5 and numNodes is 25..
     //
     // There are a number of command-line options available to control
     // the default behavior.  The list of available command-line options
     // can be listed with the following command:
     // ./waf --run "wifi-simple-adhoc-grid --help"
     //
     // Note that all ns-3 attributes (not just the ones exposed in the below
     // script) can be changed at command line; see the ns-3 documentation.
     //
     // For instance, for this configuration, the physical layer will
     // stop successfully receiving packets when distance increases beyond
     // the default of 500m.
     // To see this effect, try running:
     //
     // ./waf --run "wifi-simple-adhoc --distance=500"
     // ./waf --run "wifi-simple-adhoc --distance=1000"
     // ./waf --run "wifi-simple-adhoc --distance=1500"
     // 
     // The source node and sink node can be changed like this:
     // 
     // ./waf --run "wifi-simple-adhoc --sourceNode=20 --sinkNode=10"
     //
     // This script can also be helpful to put the Wifi layer into verbose
     // logging mode; this command will turn on all wifi logging:
     // 
     // ./waf --run "wifi-simple-adhoc-grid --verbose=1"
     //
     // By default, trace file writing is off-- to enable it, try:
     // ./waf --run "wifi-simple-adhoc-grid --tracing=1"
     //
     // When you are done tracing, you will notice many pcap trace files 
     // in your directory.  If you have tcpdump installed, you can try this:
     //
     // tcpdump -r wifi-simple-adhoc-grid-0-0.pcap -nn -tt
     //
     
     #include "ns3/core-module.h"
     #include "ns3/network-module.h"
     #include "ns3/mobility-module.h"
     #include "ns3/config-store-module.h"
     #include "ns3/wifi-module.h"
     #include "ns3/internet-module.h"
     #include "ns3/olsr-helper.h"
     #include "ns3/ipv4-static-routing-helper.h"
     #include "ns3/ipv4-list-routing-helper.h"
     #include "MyTag.h"
     #include "LRUcache.h" 
     #include <iostream>
     #include <fstream>
     #include <vector>
     #include <string>
     
     NS_LOG_COMPONENT_DEFINE ("WifiSimpleAdhocGrid");
     
     using namespace ns3;
     static Queue1* q;
     static Hasht* hasht;
     static int requestPackets[14] = {1,4,6,1,18,3,19,4,6,9,17,8,8,17}; 
     void ReceivePacket (Ptr<Socket> socket)
     {
          Ptr<Packet> recPacket = socket->Recv();
          uint8_t *buffer = new uint8_t[recPacket->GetSize()];
          recPacket->CopyData(buffer,recPacket->GetSize());
          std::string recData = std::string((char*)buffer);
          
          MyTag recTag;
          recPacket->PeekPacketTag(recTag);
       	  int tagVal =int(recTag.GetSimpleValue());
          std::ostringstream s;
          s<<tagVal;
          std::string ss(s.str());
          QNode* qN = q->front;
           int check = 0;
	  while(qN!=NULL){
              //  std::ostringstream qval;
              //  qval << qN->pageNumber;
                  
               //   NS_LOG_UNCOND(qval.str());
                  if(qN->pageNumber==tagVal){
                     check = 1;
                     NS_LOG_UNCOND("Already has packet with TagID: "+ss);
                     break;
                }
          qN = qN->next; 
          }
           ReferencePage(q,hasht,tagVal); 
          if(check==0){ 
            // ReferencePage(q,hasht,tagVal);
          
          NS_LOG_UNCOND ("Received one packet: Data: " +recData+"   TagID: "+ss);
     }}
    static std::string messages[20];
    //static Hasht* hasht;
    //static Queue1* q;
 
     static void GenerateTraffic (Ptr<Socket> socket, uint32_t pktSize, 
                                  uint32_t pktCount, Time pktInterval , int i)
     {
      if (pktCount > 0)
        {
          std::string msgx = messages[requestPackets[i]]; 
          Ptr<Packet> sendPacket =
                  Create<Packet> ((uint8_t*)msgx.c_str(),pktSize);
          MyTag sendTag;
          sendTag.SetSimpleValue(requestPackets[i]);
          sendPacket->AddPacketTag(sendTag); 
          socket->Send (sendPacket);
          NS_LOG_UNCOND ("Sending one packet: "+msgx);  
          if(i<13){ Simulator::Schedule (pktInterval, &GenerateTraffic, 
                               socket, pktSize,pktCount-1, pktInterval, i+1);}
        }
      else
        {
          socket->Close ();
        }
    }
    
    
    int main (int argc, char *argv[])
    {
      messages[0]="UCLA";
      messages[1]="MIT"; messages[2]="Stanford"; messages[3]="Berkley";
      messages[4]="UC Irvine"; messages[5]="UC San Diego"; messages[6]="USC";
      messages[7]="Carnegie Mellon University"; messages[8]="UPenn";
      messages[9]="Columbia University"; messages[10]="Cornell University";
      messages[11]="Georgia Tech"; messages[12]="TAMU"; messages[13]="NYU";
      messages[14]="UT Austin"; messages[15]="Arizona State University";
      messages[16]="NCSU"; messages[17]="University of Maryland,College Park";
      messages[18]="Stony Brook University"; 
      messages[19]="University of Indiana,Bloomington";
      
      q=createQueue(5);
      hasht = createHash(20); 
      
      std::string phyMode ("DsssRate1Mbps");
      double distance = 500;  // m
      uint32_t packetSize = 1000; // bytes
      uint32_t numPackets = 20;
      uint32_t numNodes = 25;  // by default, 5x5
      uint32_t sinkNode = 0;
      uint32_t sourceNode = 24;
      double interval = 1.0; // seconds
      bool verbose = false;
      bool tracing = false;
    
      CommandLine cmd;
    
      cmd.AddValue ("phyMode", "Wifi Phy mode", phyMode);
      cmd.AddValue ("distance", "distance (m)", distance);
      cmd.AddValue ("packetSize", "size of application packet sent", packetSize);
      cmd.AddValue ("numPackets", "number of packets generated", numPackets);
      cmd.AddValue ("interval", "interval (seconds) between packets", interval);
      cmd.AddValue ("verbose", "turn on all WifiNetDevice log components", verbose);
      cmd.AddValue ("tracing", "turn on ascii and pcap tracing", tracing);
      cmd.AddValue ("numNodes", "number of nodes", numNodes);
      cmd.AddValue ("sinkNode", "Receiver node number", sinkNode);
      cmd.AddValue ("sourceNode", "Sender node number", sourceNode);
    
      cmd.Parse (argc, argv);
      // Convert to time object
      Time interPacketInterval = Seconds (interval);
    
      // disable fragmentation for frames below 2200 bytes
      Config::SetDefault ("ns3::WifiRemoteStationManager::FragmentationThreshold", StringValue ("2200"));
      // turn off RTS/CTS for frames below 2200 bytes
      Config::SetDefault ("ns3::WifiRemoteStationManager::RtsCtsThreshold", StringValue ("2200"));
      // Fix non-unicast data rate to be the same as that of unicast
      Config::SetDefault ("ns3::WifiRemoteStationManager::NonUnicastMode", 
                          StringValue (phyMode));
    
      NodeContainer c;
      c.Create (numNodes);
    
      // The below set of helpers will help us to put together the wifi NICs we want
      WifiHelper wifi;
      if (verbose)
        {
          wifi.EnableLogComponents ();  // Turn on all Wifi logging
        }
    
      YansWifiPhyHelper wifiPhy =  YansWifiPhyHelper::Default ();
      // set it to zero; otherwise, gain will be added
      wifiPhy.Set ("RxGain", DoubleValue (-10) ); 
      // ns-3 supports RadioTap and Prism tracing extensions for 802.11b
      wifiPhy.SetPcapDataLinkType (YansWifiPhyHelper::DLT_IEEE802_11_RADIO); 
    
      YansWifiChannelHelper wifiChannel;
      wifiChannel.SetPropagationDelay ("ns3::ConstantSpeedPropagationDelayModel");
      wifiChannel.AddPropagationLoss ("ns3::FriisPropagationLossModel");
      wifiPhy.SetChannel (wifiChannel.Create ());
    
      // Add a non-QoS upper mac, and disable rate control
      NqosWifiMacHelper wifiMac = NqosWifiMacHelper::Default ();
      wifi.SetStandard (WIFI_PHY_STANDARD_80211b);
      wifi.SetRemoteStationManager ("ns3::ConstantRateWifiManager",
                                    "DataMode",StringValue (phyMode),
                                    "ControlMode",StringValue (phyMode));
      // Set it to adhoc mode
      wifiMac.SetType ("ns3::AdhocWifiMac");
      NetDeviceContainer devices = wifi.Install (wifiPhy, wifiMac, c);
    
      MobilityHelper mobility;
      mobility.SetPositionAllocator ("ns3::GridPositionAllocator",
                                     "MinX", DoubleValue (0.0),
                                     "MinY", DoubleValue (0.0),
                                     "DeltaX", DoubleValue (distance),
                                     "DeltaY", DoubleValue (distance),
                                     "GridWidth", UintegerValue (5),
                                     "LayoutType", StringValue ("RowFirst"));
      mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
      mobility.Install (c);
    
      // Enable OLSR
      OlsrHelper olsr;
      Ipv4StaticRoutingHelper staticRouting;
    
      Ipv4ListRoutingHelper list;
      list.Add (staticRouting, 0);
      list.Add (olsr, 10);
    
      InternetStackHelper internet;
      internet.SetRoutingHelper (list); // has effect on the next Install ()
      internet.Install (c);
    
      Ipv4AddressHelper ipv4;
      NS_LOG_INFO ("Assign IP Addresses.");
      ipv4.SetBase ("10.1.1.0", "255.255.255.0");
      Ipv4InterfaceContainer i = ipv4.Assign (devices);
    
      TypeId tid = TypeId::LookupByName ("ns3::UdpSocketFactory");
      Ptr<Socket> recvSink = Socket::CreateSocket (c.Get (sinkNode), tid);
      InetSocketAddress local = InetSocketAddress (Ipv4Address::GetAny (), 80);
      recvSink->Bind (local);
      recvSink->SetRecvCallback (MakeCallback (&ReceivePacket));
    
      Ptr<Socket> source = Socket::CreateSocket (c.Get (sourceNode), tid);
      InetSocketAddress remote = InetSocketAddress (i.GetAddress (sinkNode, 0), 80);
      source->Connect (remote);
    
      if (tracing == true)
        {
          AsciiTraceHelper ascii;
          wifiPhy.EnableAsciiAll (ascii.CreateFileStream ("wifi-simple-adhoc-grid.tr"));
          wifiPhy.EnablePcap ("wifi-simple-adhoc-grid", devices);
          // Trace routing tables
          Ptr<OutputStreamWrapper> routingStream = Create<OutputStreamWrapper> ("wifi-simple-adhoc-grid.routes", std::ios::out);
          olsr.PrintRoutingTableAllEvery (Seconds (2), routingStream);
    
          // To do-- enable an IP-level trace that shows forwarding events only
        }
    
      // Give OLSR time to converge-- 30 seconds perhaps
      Simulator::Schedule (Seconds (30.0), &GenerateTraffic, 
                           source, packetSize, numPackets, interPacketInterval,0);
    
      // Output what we are doing
      NS_LOG_UNCOND ("Testing from node " << sourceNode << " to " << sinkNode << " with grid distance " << distance);
    
      Simulator::Stop (Seconds (132.0));
      Simulator::Run ();
      Simulator::Destroy ();
    
      return 0;
    }