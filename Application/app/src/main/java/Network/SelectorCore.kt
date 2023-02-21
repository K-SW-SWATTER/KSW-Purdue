package Network

import java.nio.channels.SelectionKey
import java.nio.channels.Selector

class SelectorCore
{
    private val _selector: Selector = Selector.open();


    fun GetSelector(): Selector? { return _selector; }
    fun Dispatch()
    {
        while (_selector.select() > 0)
        {
            var keys = _selector.selectedKeys().iterator()
            while (keys.hasNext())
            {
                var key = keys.next();

                keys.remove();

                if (key.isValid() == false)
                    continue;

                if (key.isReadable())
                {
                    ServerPacketHandler.HandlePacket(key)
                }
            }
        }
    }
}