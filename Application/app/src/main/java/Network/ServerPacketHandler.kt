package Network

import java.nio.ByteBuffer
import java.nio.channels.SelectionKey
import java.nio.channels.SocketChannel

enum class PacketID(val id: UInt)
{
    C_LOGIN(1000U),
    S_LOGIN(1001U),
    C_AUDIO_DATA(1002U),
    S_DETECTION_RESULT(1003U),
}

enum class DetectionResult(val id: UInt)
{
    autel_evo_2(0U),
    DJI_Phantom_4(1U),
    Noise(2U),
}

class ServerPacketHandler
{
    companion object
    {
        fun HandlePacket(key: SelectionKey)
        {
            try
            {
                var channel = key.channel() as SocketChannel;
                var socket = channel.socket();
                channel.configureBlocking(false); // Make non-block

                var buffer = ByteBuffer.allocate(128);
                var bufSize = channel.read(buffer);

                if (bufSize == -1) // Failed with read
                {
                    var remoteAddress = socket.remoteSocketAddress;
                    println("Disconnected by Client" + remoteAddress);
                    channel.close();
                    socket.close();
                    key.cancel();
                    return;
                }
                var pktSize = buffer.getInt();
                var pktId = buffer.getInt();

                var pkt: Packet = PacketFactory.GetPacket(pktSize, pktId); // Get the packet by id through PacketFactory


                when (pkt.id)
                {
                    PacketID.S_DETECTION_RESULT.id -> Handle_S_DETECTION_RESULT(pkt);
                }
            }
            catch (e: Exception)
            {
                println(e);
            }
        }

        fun Handle_S_DETECTION_RESULT(pkt: PKT_S_DETECTION_RESULT): UInt
        {
            when(pkt.result)
            {
                DetectionResult.autel_evo_2.id -> println("autel evo 2");
                DetectionResult.DJI_Phantom_4.id -> println("DJI Phantom 4");
                DetectionResult.Noise.id -> println("Noise");
            }
        }
    }
}