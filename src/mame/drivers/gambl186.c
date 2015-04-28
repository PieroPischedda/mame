/* Unknown Gambling game with 186 CPU */

/*

80186xl20   Xtal 40Mhz
At89c52 (not dumped) with external 32K ram?? Xtal 11.xxx18Mhz
Cirrus Logic CL-GD5428-80QC-A video chip with 416c256 ram near it Xtal 14.x818 Mhaz

U9-U10 programm rom common to both pcb
u11-u12 Program rom/GFX
U13-U14 256Kramx8 (32Kbyte x16)

U23 Nec D71055C
U28 Nec D71055C
U22 Nec D71055C
U42 Nec D71051C
U500 Nec D71051C (not present on board "1")

U3 Max691cpe

U300 Nec D7759GC (10Mhz xtal near it)

TODO:
- No idea about how to surpass the "No Funzione" (sic) screen. According to the bad settings almost
  surely it wants EEPROM hooked up (i/o at 0x680/0x682?)

*/



#include "emu.h"
#include "cpu/i86/i186.h"
#include "video/pc_vga.h"


class gambl186_state : public driver_device
{
public:
	gambl186_state(const machine_config &mconfig, device_type type, const char *tag)
		: driver_device(mconfig, type, tag),
		m_maincpu(*this, "maincpu") { }

	required_device<cpu_device> m_maincpu;
    int m_comms_state;
	int m_comms_ind;
	UINT8 m_comms_data[1002];

	DECLARE_READ16_MEMBER(comms_r);
	DECLARE_WRITE16_MEMBER(comms_w);
};



static ADDRESS_MAP_START( gambl186_map, AS_PROGRAM, 16, gambl186_state )
	AM_RANGE(0x00000, 0x0ffff) AM_RAM
	AM_RANGE(0x40000, 0x4ffff) AM_ROM AM_REGION("data",0) // TODO: way bigger than this, banked?
	AM_RANGE(0xa0000, 0xbffff) AM_DEVREADWRITE8("vga", cirrus_gd5428_device, mem_r, mem_w, 0xffff)
	AM_RANGE(0xc0000, 0xfffff) AM_ROM AM_REGION("ipl",0)
ADDRESS_MAP_END

READ16_MEMBER(gambl186_state::comms_r)
{
    if ((offset == 0) && ACCESSING_BITS_0_7) //port 680 == data
    {
        if ((m_comms_state == 0x16) && (m_comms_ind < sizeof(m_comms_data)))
        {
            return m_comms_data[m_comms_ind++];
        }
    }
    else if (offset == 1) //port 681 == status
    {
        if (m_comms_state == 0x16) //read mode?
        {
            return 2;
        }
        else if (m_comms_state == 0x31) //write mode?
        {
            return 4;
        }
    }

    return 0;
}

WRITE16_MEMBER(gambl186_state::comms_w)
{
    if (offset == 0)
    {
        if ((m_comms_state == 0x31) && (m_comms_ind < (sizeof(m_comms_data) - 2)))
        {
            m_comms_data[++m_comms_ind] = (UINT8) ~data;
        }
    }
    else if (offset == 1)
    {
        if (m_comms_state != data)
        {
            m_comms_ind = 0;
        }

        m_comms_state = data;

        if (data == 0x4e) //reset?
        {
            m_comms_data[0] = 5;
            m_comms_data[sizeof(m_comms_data) - 1] = 0xec;
        }
    }
}

static ADDRESS_MAP_START( gambl186_io, AS_IO, 16, gambl186_state )
	AM_RANGE(0x03b0, 0x03bf) AM_DEVREADWRITE8("vga", cirrus_gd5428_device, port_03b0_r, port_03b0_w, 0xffff)
	AM_RANGE(0x03c0, 0x03cf) AM_DEVREADWRITE8("vga", cirrus_gd5428_device, port_03c0_r, port_03c0_w, 0xffff)
	AM_RANGE(0x03d0, 0x03df) AM_DEVREADWRITE8("vga", cirrus_gd5428_device, port_03d0_r, port_03d0_w, 0xffff)
	AM_RANGE(0x0400, 0x0401) AM_WRITENOP // sound
	AM_RANGE(0x0500, 0x0501) AM_READ_PORT("IN0")
	AM_RANGE(0x0502, 0x0503) AM_READ_PORT("IN1")
	AM_RANGE(0x0504, 0x0505) AM_READ_PORT("IN2")

	//AM_RANGE(0x0500, 0x050f) AM_READ(unk_r)
	AM_RANGE(0x0580, 0x0581) AM_READ_PORT("DSW0")
	AM_RANGE(0x0582, 0x0583) AM_READ_PORT("DSW1")
	AM_RANGE(0x0584, 0x0585) AM_READ_PORT("DSW2") AM_WRITENOP // ???
	AM_RANGE(0x0600, 0x0603) AM_WRITENOP // lamps
	AM_RANGE(0x0680, 0x0683) AM_READWRITE(comms_r, comms_w)
ADDRESS_MAP_END



static INPUT_PORTS_START( gambl186 )
	PORT_START("IN0")
	PORT_DIPNAME( 0x01, 0x01, "0-1" )
	PORT_DIPSETTING(    0x01, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x02, 0x02, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x02, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x04, 0x04, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x04, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x08, 0x08, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x08, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x10, 0x10, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x10, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x20, 0x20, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x20, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x40, 0x40, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x40, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x80, 0x80, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x80, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_BIT( 0x0100, IP_ACTIVE_LOW, IPT_POKER_HOLD1 )
	PORT_BIT( 0x0200, IP_ACTIVE_LOW, IPT_POKER_HOLD2 )
	PORT_BIT( 0x0400, IP_ACTIVE_LOW, IPT_POKER_HOLD3 )
	PORT_BIT( 0x0800, IP_ACTIVE_LOW, IPT_POKER_HOLD4 )
	PORT_BIT( 0x1000, IP_ACTIVE_LOW, IPT_POKER_HOLD5 )
	PORT_BIT( 0x2000, IP_ACTIVE_LOW, IPT_START1 )
	PORT_DIPNAME( 0x4000, 0x4000, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x4000, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x0000, DEF_STR( On ) )
	PORT_BIT( 0x8000, IP_ACTIVE_LOW, IPT_START2 ) PORT_NAME("-") // Unknown meaning

	PORT_START("IN1")
	PORT_DIPNAME( 0x01, 0x01, "1-1" )
	PORT_DIPSETTING(    0x01, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x02, 0x02, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x02, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x04, 0x04, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x04, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x08, 0x08, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x08, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x10, 0x10, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x10, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x20, 0x20, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x20, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x40, 0x40, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x40, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x80, 0x80, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x80, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x0100, 0x0100, "1-2" )
	PORT_DIPSETTING(    0x0100, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x0200, 0x0200, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x0200, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x0400, 0x0400, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x0400, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x0800, 0x0800, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x0800, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x1000, 0x1000, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x1000, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x2000, 0x2000, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x2000, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x4000, 0x4000, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x4000, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x8000, 0x8000, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x8000, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x0000, DEF_STR( On ) )

	PORT_START("IN2")
	PORT_DIPNAME( 0x01, 0x01, "2-1" )
	PORT_DIPSETTING(    0x01, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x02, 0x02, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x02, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x04, 0x04, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x04, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x08, 0x08, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x08, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x10, 0x10, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x10, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x20, 0x20, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x20, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x40, 0x40, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x40, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x80, 0x80, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x80, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	/* TODO: order isn't honored */
	PORT_BIT( 0x0100, IP_ACTIVE_LOW, IPT_COIN1 )
	PORT_BIT( 0x0200, IP_ACTIVE_LOW, IPT_COIN2 )
	PORT_BIT( 0x0400, IP_ACTIVE_LOW, IPT_COIN3 )
	PORT_BIT( 0x0800, IP_ACTIVE_LOW, IPT_COIN4 )
	PORT_BIT( 0x1000, IP_ACTIVE_LOW, IPT_COIN5 )
	PORT_BIT( 0x2000, IP_ACTIVE_LOW, IPT_COIN6 )
	PORT_DIPNAME( 0x4000, 0x4000, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x4000, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x0000, DEF_STR( On ) )
	PORT_BIT( 0x8000, IP_ACTIVE_LOW, IPT_GAMBLE_KEYIN )

	PORT_START("DSW0")
	PORT_DIPNAME( 0x01, 0x01, "0-1" )
	PORT_DIPSETTING(    0x01, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x02, 0x02, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x02, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x04, 0x04, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x04, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x08, 0x08, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x08, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x10, 0x10, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x10, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x20, 0x20, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x20, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x40, 0x40, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x40, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x80, 0x80, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x80, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x0100, 0x0100, "0-2" )
	PORT_DIPSETTING(    0x0100, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x0200, 0x0200, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x0200, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x0400, 0x0400, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x0400, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x0800, 0x0800, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x0800, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x1000, 0x1000, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x1000, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x2000, 0x2000, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x2000, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x4000, 0x4000, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x4000, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x8000, 0x8000, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x8000, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x0000, DEF_STR( On ) )

	PORT_START("DSW1")
	PORT_DIPNAME( 0x01, 0x01, "1-1" )
	PORT_DIPSETTING(    0x01, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x02, 0x02, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x02, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x04, 0x04, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x04, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x08, 0x08, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x08, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x10, 0x10, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x10, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x20, 0x20, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x20, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x40, 0x40, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x40, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x80, 0x80, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x80, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x0100, 0x0100, "1-2" )
	PORT_DIPSETTING(    0x0100, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x0200, 0x0200, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x0200, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x0400, 0x0400, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x0400, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x0800, 0x0800, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x0800, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x1000, 0x1000, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x1000, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x2000, 0x2000, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x2000, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x4000, 0x4000, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x4000, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x8000, 0x8000, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x8000, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x0000, DEF_STR( On ) )

	PORT_START("DSW2")
	PORT_DIPNAME( 0x01, 0x01, "2-1" )
	PORT_DIPSETTING(    0x01, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x02, 0x02, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x02, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x04, 0x04, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x04, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x08, 0x08, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x08, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x10, 0x10, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x10, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x20, 0x20, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x20, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x40, 0x40, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x40, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x80, 0x80, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x80, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x0100, 0x0100, "2-2" )
	PORT_DIPSETTING(    0x0100, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x0200, 0x0200, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x0200, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x0400, 0x0400, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x0400, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x0800, 0x0800, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x0800, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x0000, DEF_STR( On ) )
	PORT_SERVICE(0x1000, IP_ACTIVE_LOW )

	PORT_DIPNAME( 0x2000, 0x2000, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x2000, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x4000, 0x4000, "Bookkeeping" )
	PORT_DIPSETTING(    0x4000, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x8000, 0x8000, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x8000, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x0000, DEF_STR( On ) )

INPUT_PORTS_END



static MACHINE_CONFIG_START( gambl186, gambl186_state )
	MCFG_CPU_ADD("maincpu", I80186, XTAL_40MHz/2)
	MCFG_CPU_PROGRAM_MAP(gambl186_map)
	MCFG_CPU_IO_MAP(gambl186_io)

	MCFG_FRAGMENT_ADD( pcvideo_cirrus_gd5428 )
MACHINE_CONFIG_END




ROM_START( gambl186 )
	ROM_REGION( 0x100000, "data", 0 )
	ROM_LOAD16_BYTE( "ie398.u11", 0x00000, 0x80000, CRC(86ad7cab) SHA1(b701c3701db630d218a9b1700f216f795a1b1272) )
	ROM_LOAD16_BYTE( "io398.u12", 0x00001, 0x80000, CRC(0a036f34) SHA1(63d0b87c7d4c902413f28c0b55d78e5fda511f4f) )

	ROM_REGION( 0x40000, "ipl", 0 )
	ROM_LOAD16_BYTE( "se403p.u9",  0x00000, 0x20000, CRC(1021cc20) SHA1(d9bb67676b05458ff813d608431ff06946ab7721) )
	ROM_LOAD16_BYTE( "so403p.u10", 0x00001, 0x20000, CRC(af9746c9) SHA1(3f1ab8110cc5eadec661181779799693ad695e21) )

	ROM_REGION( 0x20000, "snd", 0 )
	ROM_LOAD( "347.u302", 0x00000, 0x20000, CRC(7ce8f490) SHA1(2f856e31d189e9d46ba6b322133d99133e0b52ac) )
ROM_END

ROM_START( gambl186a )
	ROM_REGION( 0x100000, "data", 0 )
	ROM_LOAD16_BYTE( "ie399.u11", 0x00000, 0x80000, CRC(2a7bce20) SHA1(fbabaaa0d72b5dfccd33f5194d13009bdc44b5a7) )
	ROM_LOAD16_BYTE( "io399.u12", 0x00001, 0x80000, CRC(9212f52b) SHA1(d970c59c1e0f5f7e94c1b632398bcfae278c143d) )

	ROM_REGION( 0x40000, "ipl", 0 )
	ROM_LOAD16_BYTE( "se403p.u9",  0x00000, 0x20000, CRC(1021cc20) SHA1(d9bb67676b05458ff813d608431ff06946ab7721) )
	ROM_LOAD16_BYTE( "so403p.u10", 0x00001, 0x20000, CRC(af9746c9) SHA1(3f1ab8110cc5eadec661181779799693ad695e21) )

	ROM_REGION( 0x20000, "snd", 0 )
	ROM_LOAD( "347.u302", 0x00000, 0x20000, CRC(7ce8f490) SHA1(2f856e31d189e9d46ba6b322133d99133e0b52ac) )
ROM_END


GAME( 1999, gambl186,  0,        gambl186,   gambl186, driver_device,   0,       ROT0,  "<unknown>", "unknown 186 based gambling game (V398)",         GAME_NOT_WORKING | GAME_NO_SOUND )
GAME( 199?, gambl186a, gambl186, gambl186,   gambl186, driver_device,   0,       ROT0,  "<unknown>", "unknown 186 based gambling game (V399)",         GAME_NOT_WORKING | GAME_NO_SOUND )
