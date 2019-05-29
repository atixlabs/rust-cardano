pub type Error = u32;

#[repr(C)]
pub enum KindType {
    Single,
    Group,
    Account,
    Multisig,
}

impl From<chain_addr::KindType> for KindType {
    fn from(kind: chain_addr::KindType) -> Self {
        match kind {
            chain_addr::KindType::Single => KindType::Single,
            chain_addr::KindType::Group => KindType::Group,
            chain_addr::KindType::Account => KindType::Account,
            chain_addr::KindType::Multisig => KindType::Multisig,
        }
    }
}

pub struct Address(chain_addr::Address);

impl From<chain_addr::Address> for Address {
    fn from(addr: chain_addr::Address) -> Self {
        Address(addr)
    }
}

impl AsMut<chain_addr::Address> for Address {
    fn as_mut(&mut self) -> &mut chain_addr::Address {
        &mut self.0
    }
}