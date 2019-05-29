use chain_addr::AddressReadable;
use std::ffi;
use std::ptr;
use std::ffi::CString;
use types::{ Error, KindType, Address };
use std::os::raw::{c_char};

#[no_mangle]
pub extern "C" fn chain_address_from_bytes(
    bytes: *const u8,
    address: *mut *const Address, 
) -> Error {
    unimplemented!()
}

#[no_mangle]
pub extern "C" fn chain_address_to_bytes(
    address: *mut Address,
    out_ptr: *mut *mut u8,
    out_size: *mut usize
) {
    let address = unsafe { address.as_mut() }.expect("Not a NULL PTR");
    let mut slice = address.as_mut().to_bytes().into_boxed_slice();

    let ptr = slice.as_mut_ptr();
    let size = slice.len();

    std::mem::forget(slice);

    unsafe { ptr::write(out_ptr, ptr) };
    unsafe { ptr::write(out_size, size) };
}

#[no_mangle]
pub extern "C" fn chain_address_bytes_delete(
    ptr: *mut u8,
    size: usize
) {
    unsafe {
        let slice = std::slice::from_raw_parts_mut(ptr, size);
        Box::from_raw(slice);
    };
}

#[no_mangle]
pub extern "C" fn chain_address_to_size(address: *mut Address) -> usize {
    unimplemented!()
}

#[no_mangle]
pub extern "C" fn chain_address_to_kind_type(address: *mut Address) -> KindType {
    unimplemented!()
}

#[no_mangle]
pub extern "C" fn chain_address_public_key(address: *mut Address) -> *mut u8 {
    unimplemented!()
}

#[no_mangle]
pub extern "C" fn chain_address_base32(address: *const Address) -> *const c_char {
    unimplemented!()
}

#[no_mangle]
pub extern "C" fn chain_address_to_readable(address: *const Address) -> *const c_char {
    unimplemented!()
}

#[no_mangle]
pub extern "C" fn chain_readable_address_delete(c_str: *mut c_char) {
    unsafe { CString::from_raw(c_str) };
}

#[no_mangle]
pub extern "C" fn chain_address_from_readable(str_address: *const c_char) -> *mut Address {
    let address = unsafe { ffi::CStr::from_ptr(str_address) }.to_string_lossy();
    let readable = match AddressReadable::from_string(&address) {
        Ok(address) => address,
        Err(_) => unimplemented!() 
    };
    let ptr = Box::new(readable.to_address().into());
    Box::into_raw(ptr)
}